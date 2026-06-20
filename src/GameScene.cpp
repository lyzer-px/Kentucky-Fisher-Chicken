/*
** EPITECH PROJECT, 2026
** GameScene.cpp
** File description:
** GameScene class — diving game logic
*/

#include "GameScene.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <numeric>

static constexpr float PI = 3.14159265f;
static constexpr float W  = 800.f;
static constexpr float H  = 600.f;

namespace GraphLib {

// ─── helpers ─────────────────────────────────────────────────────────────────

static float randf(float lo, float hi) {
    return lo + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

static sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return sf::Color(
        static_cast<sf::Uint8>(a.r + (b.r - a.r) * t),
        static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
        static_cast<sf::Uint8>(a.b + (b.b - a.b) * t)
    );
}

// ─── type picker (weighted random) ───────────────────────────────────────────

SphereType GameScene::pickType() const
{
    int total = 0;
    for (const auto& info : SPHERE_TYPES) total += info.weight;
    int roll = static_cast<int>(randf(0.f, static_cast<float>(total)));
    int acc  = 0;
    for (std::size_t i = 0; i < SPHERE_TYPES.size(); ++i) {
        acc += SPHERE_TYPES[i].weight;
        if (roll < acc) return static_cast<SphereType>(i);
    }
    return SphereType::SMALL;
}

Prey GameScene::makePrey() const
{
    Prey p;
    p.type = pickType();
    const auto& info = SPHERE_TYPES[static_cast<int>(p.type)];

    p.entity = std::make_unique<Entity>(
        _game.getTexture(info.textureName),
        info.textureName
    );
    p.entity->centerOrigin();
    p.entity->setSize({info.radius * 2.f, info.radius * 2.f});
    float margin = info.radius + 10.f;
    float x = randf(margin, W - margin);
    float y = randf(_waterY + margin, H - margin);
    p.entity->setPosition({x, y});
    p.angle       = randf(0.f, 2.f * PI);
    p.targetAngle = p.angle;
    p.speed       = randf(_preySpeed * 0.6f, _preySpeed * 1.4f);
    p.wanderTimer = randf(1.5f, 4.f);
    p.velocity    = {std::cos(p.angle) * p.speed, std::sin(p.angle) * p.speed};
    return p;
}

void GameScene::spawnPrey()
{
    _prey.clear();
    int count = targetPreyCount();
    for (int i = 0; i < count; ++i)
        _prey.push_back(makePrey());
}

// More fish when sea is high (waterY is a smaller number = higher on screen)
int GameScene::targetPreyCount() const
{
    float seaRange  = SEA_MAX_Y - SEA_MIN_Y;
    float highness  = (_targetWaterY - SEA_MIN_Y) / seaRange; // 0=max high, 1=max low
    // base + up to 8 extra when fully high
    int bonus = static_cast<int>((1.f - highness) * 8.f);
    return _preyCount + bonus;
}

// ─── sea level ───────────────────────────────────────────────────────────────

void GameScene::updateSea(float dt)
{
    // Animate surface toward target
    if (std::abs(_waterY - _targetWaterY) > 1.f) {
        float dir = (_targetWaterY > _waterY) ? 1.f : -1.f;
        float step = SEA_ANIM_SPEED * dt;
        if (step >= std::abs(_targetWaterY - _waterY))
            _waterY = _targetWaterY;
        else
            _waterY += dir * step;
        updateWaterShape();
    }

    // Schedule next shift
    _seaShiftTimer -= dt;
    if (_seaShiftTimer <= 0.f) {
        float shift = randf(SEA_SHIFT_MIN, SEA_SHIFT_MAX);
        // Randomly go up or down, but keep within bounds
        bool goUp = randf(0.f, 1.f) < 0.5f;
        float candidate = _targetWaterY + (goUp ? -shift : shift);
        _targetWaterY = std::clamp(candidate, SEA_MIN_Y, SEA_MAX_Y);
        _seaShiftTimer = randf(SEA_SHIFT_INTERVAL_MIN, SEA_SHIFT_INTERVAL_MAX);
        // Extra fish will be spawned by updatePrey's replenish loop this frame
    }
}

void GameScene::updateWaterShape()
{
    _water.setPosition(0.f, _waterY);
    _water.setSize({W, H - _waterY});

    // Also update hover Y for red sphere if hovering
    if (_redState == RedState::HOVERING)
        _redPos.y = _waterY - HOVER_OFFSET;
}

void GameScene::cullDrownedPrey()
{
    for (auto& p : _prey) {
        if (!p.alive) continue;
        float top = p.entity->getPosition().y - SPHERE_TYPES[static_cast<int>(p.type)].radius;
        if (top < _waterY) {
            p.alive = false;
            p.entity->setVisible(false);
        }
    }
}

// ─── prey update ─────────────────────────────────────────────────────────────

void GameScene::updatePrey(float dt)
{
    static constexpr float TURN_SPEED = 2.2f; // radians/s max turn rate

    for (auto& p : _prey) {
        if (!p.alive) continue;
        const float R = SPHERE_TYPES[static_cast<int>(p.type)].radius;

        // ── wander: pick a new target angle periodically ──
        p.wanderTimer -= dt;
        if (p.wanderTimer <= 0.f) {
            // Drift by at most ~90 degrees, biased to keep swimming
            p.targetAngle += randf(-PI * 0.6f, PI * 0.6f);
            p.wanderTimer  = randf(1.5f, 4.f);
        }

        // ── smooth steering: rotate current angle toward target ──
        float diff = p.targetAngle - p.angle;
        // Wrap diff to [-PI, PI]
        while (diff >  PI) diff -= 2.f * PI;
        while (diff < -PI) diff += 2.f * PI;
        float maxTurn = TURN_SPEED * dt;
        if (std::abs(diff) <= maxTurn)
            p.angle = p.targetAngle;
        else
            p.angle += (diff > 0.f ? maxTurn : -maxTurn);

        p.velocity = {std::cos(p.angle) * p.speed, std::sin(p.angle) * p.speed};

        sf::Vector2f pos = p.entity->getPosition();
        pos += p.velocity * dt;

        // ── wall bounce: reflect angle instead of snapping velocity ──
        if (pos.x - R < 0.f)    { pos.x = R;           p.angle = PI - p.angle;       p.targetAngle = p.angle; }
        if (pos.x + R > W)       { pos.x = W - R;       p.angle = PI - p.angle;       p.targetAngle = p.angle; }
        if (pos.y - R < _waterY) { pos.y = _waterY + R; p.angle = -p.angle;           p.targetAngle = p.angle; }
        if (pos.y + R > H)       { pos.y = H - R;       p.angle = -p.angle;           p.targetAngle = p.angle; }

        p.entity->setPosition(pos);
        p.entity->setRotation(p.angle * 180.f / PI);

        // ── depth tint: grey out prey below the reachable zone ──
        float depth = pos.y - _waterY;
        if (depth > MAX_DIVE_DEPTH) {
            float excess = std::clamp((depth - MAX_DIVE_DEPTH) / 80.f, 0.f, 1.f);
            sf::Color grey = lerpColor(sf::Color::White, sf::Color(70, 70, 80), excess);
            p.entity->setColor(grey);
        } else {
            p.entity->setColor(sf::Color::White);
        }
    }

    // Replenish to match sea-level-aware target count
    int target = targetPreyCount();
    int alive  = 0;
    for (const auto& p : _prey) alive += p.alive ? 1 : 0;
    while (alive < target) {
        _prey.push_back(makePrey());
        ++alive;
    }
}

// ─── red sphere ──────────────────────────────────────────────────────────────

void GameScene::updateRedSphere(float dt)
{
    if (_redState == RedState::HOVERING) {
        _bobTimer += dt;
        float hoverY = (_waterY - HOVER_OFFSET) + std::sin(_bobTimer * 2.f) * 6.f;
        _redPos.y = hoverY;
        _redSphere.setPosition(_redPos);
        if (_chicken) {
            _chicken->setPosition(_redPos);
            _chicken->setRotation(0.f);
            _chicken->update(dt);
        }
        return;
    }
    sf::Vector2f dir = _redTarget - _redPos;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    float spd = (_redState == RedState::DIVING) ? DIVE_SPEED : RETURN_SPEED;
    if (dist < spd * dt) {
        _redPos = _redTarget;
        _redSphere.setPosition(_redPos);
        if (_chicken) {
            _chicken->setPosition(_redPos);
            if (dist > 0.1f) {
                float angleDeg = std::atan2(dir.y, dir.x) * 180.f / PI;
                _chicken->setRotation(angleDeg);
            }
            _chicken->update(dt);
        }
        if (_redState == RedState::DIVING) {
            _redTarget = {_redPos.x, _waterY - HOVER_OFFSET};
            _redState = RedState::RETURNING;
        } else {
            _redPos.x = W / 2.f;
            _redTarget = _redPos;
            _redState = RedState::HOVERING;
            _bobTimer = 0.f;

            if (_chicken) {
                _chicken->setPosition(_redPos);
                _chicken->setRotation(0.f);
            }
        }
        return;
    }
    dir /= dist;
    _redPos += dir * spd * dt;
    _redSphere.setPosition(_redPos);
    if (_chicken) {
        _chicken->setPosition(_redPos);
        float angleDeg = std::atan2(dir.y, dir.x) * 180.f / PI;
        _chicken->setRotation(angleDeg);
        _chicken->update(dt);
    }
}

// ─── click ───────────────────────────────────────────────────────────────────

int GameScene::findClickedPrey(sf::Vector2f pos) const
{
    for (int i = 0; i < (int)_prey.size(); ++i) {
        if (!_prey[i].alive) continue;
        const float R = SPHERE_TYPES[static_cast<int>(_prey[i].type)].radius;
        sf::Vector2f d = pos - _prey[i].entity->getPosition();
        if (std::sqrt(d.x * d.x + d.y * d.y) <= R + 6.f)
            return i;
    }
    return -1;
}

void GameScene::handleClick(sf::Vector2f pos)
{
    if (_gameOver || _redState != RedState::HOVERING) return;
    int idx = findClickedPrey(pos);
    if (idx == -1) return;
    sf::Vector2f target = _prey[idx].entity->getPosition();
    float        depth  = target.y - _waterY;
    _redTarget = target;
    _redState  = RedState::DIVING;
    if (depth <= MAX_DIVE_DEPTH) {
        int fill = SPHERE_TYPES[static_cast<int>(_prey[idx].type)].fill;
        _prey[idx].alive = false;
        _prey[idx].entity->setVisible(false);
        _hungerFill = std::min(1.f, _hungerFill + fill / static_cast<float>(CATCHES_TO_WIN));
        if (_hungerFill >= 1.f)
            levelUp();
        else
            updateBar();
    } else {
        _lives--;
        if (_lives <= 0) { _lives = 0; _gameOver = true; }
        buildHearts();
    }
}

// ─── UI ──────────────────────────────────────────────────────────────────────

sf::ConvexShape GameScene::makeHeart(sf::Vector2f center, float size, sf::Color color) const
{
    const int N = 40;
    sf::ConvexShape h;
    h.setPointCount(N);
    for (int i = 0; i < N; ++i) {
        float t = static_cast<float>(i) / N * 2.f * PI;
        float x =  size * 0.9f * std::pow(std::sin(t), 3.f);
        float y = -size * (0.8125f * std::cos(t) - 0.3125f * std::cos(2.f * t)
                        - 0.125f  * std::cos(3.f * t) - 0.0625f * std::cos(4.f * t));
        h.setPoint(i, {center.x + x, center.y + y});
    }
    h.setFillColor(color);
    h.setOutlineColor(sf::Color(100, 0, 0));
    h.setOutlineThickness(1.5f);
    return h;
}

void GameScene::buildHearts()
{
    for (int i = 0; i < MAX_LIVES; ++i) {
        sf::Color c = (i < _lives) ? sf::Color(220, 40, 60) : sf::Color(55, 55, 70);
        _hearts[i] = makeHeart({12.f + i * 36.f + 14.f, 22.f}, 12.f, c);
    }
}

void GameScene::buildGameOverX()
{
    const float CX = W / 2.f, CY = H / 2.f, LEN = 80.f, TH = 18.f;
    for (int i = 0; i < 2; ++i) {
        _gameOverX[i].setSize({LEN * 2.f, TH});
        _gameOverX[i].setOrigin(LEN, TH / 2.f);
        _gameOverX[i].setPosition(CX, CY);
        _gameOverX[i].setRotation(i == 0 ? 45.f : -45.f);
        _gameOverX[i].setFillColor(sf::Color(220, 40, 40));
    }
}

void GameScene::updateBar()
{
    float t = std::clamp(_hungerFill, 0.f, 1.f);
    sf::Color barColor;
    if (t < 0.5f)
        barColor = lerpColor(sf::Color(210, 40, 40), sf::Color(230, 150, 20), t * 2.f);
    else
        barColor = lerpColor(sf::Color(230, 150, 20), sf::Color(60, 200, 80), (t - 0.5f) * 2.f);

    _barFill.setSize({BAR_W * t, BAR_H});
    _barFill.setFillColor(barColor);
}

// ─── level up / reset ────────────────────────────────────────────────────────

void GameScene::levelUp()
{
    _level++;
    _hungerFill  = 0.f;
    _preySpeed   = 60.f + (_level - 1) * 20.f;
    _preyCount   = 6    + (_level - 1) * 2;
    spawnPrey();
    updateBar();
}

void GameScene::reset()
{
    _level       = 1;
    _hungerFill  = 0.f;
    _lives       = MAX_LIVES;
    _gameOver    = false;
    _preySpeed   = 60.f;
    _preyCount   = 6;
    _waterY      = BASE_WATER_Y;
    _targetWaterY = BASE_WATER_Y;
    _seaShiftTimer = randf(SEA_SHIFT_INTERVAL_MIN, SEA_SHIFT_INTERVAL_MAX);
    _redPos      = {W / 2.f, _waterY - HOVER_OFFSET};
    _redSphere.setPosition(_redPos);
    if (_chicken) {
        _chicken->setPosition(_redPos);
        _chicken->setRotation(0.f);
        _chicken->setColor(sf::Color::White);
        _chicken->setVisible(true);
    }
    _redState    = RedState::HOVERING;
    _bobTimer    = 0.f;
    updateWaterShape();
    spawnPrey();
    buildHearts();
    updateBar();
}

// ─── constructor ─────────────────────────────────────────────────────────────

GameScene::GameScene(Game& game)
    : AScene(game),
    _waterY(BASE_WATER_Y),
    _targetWaterY(BASE_WATER_Y),
    _seaShiftTimer(6.f),
    _level(1),
    _preySpeed(60.f),
    _preyCount(6),
    _redState(RedState::HOVERING),
    _bobTimer(0.f),
    _hungerFill(0.f),
    _lives(MAX_LIVES),
    _gameOver(false)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    _sky.setSize({W, H});
    _sky.setPosition(0, 0);
    _sky.setFillColor(sf::Color(135, 206, 235));

    _water.setFillColor(sf::Color(30, 80, 160, 210));
    updateWaterShape();

    _redPos = {W / 2.f, _waterY - HOVER_OFFSET};

    _chicken = std::make_unique<Entity>(
        _game.getTexture("chicken"),
        "chicken"
    );

    _chicken->centerOrigin();
    _chicken->setSize({RED_RADIUS * 2.f, RED_RADIUS * 2.f});
    _chicken->setPosition(_redPos);

    _redSphere.setRadius(RED_RADIUS);
    _redSphere.setOrigin(RED_RADIUS, RED_RADIUS);
    _redSphere.setFillColor(sf::Color(220, 40, 40));
    _redSphere.setOutlineColor(sf::Color(140, 10, 10));
    _redSphere.setOutlineThickness(2.f);
    _redSphere.setPosition(_redPos);

    _barBg.setSize({BAR_W, BAR_H});
    _barBg.setPosition(BAR_X, BAR_Y);
    _barBg.setFillColor(sf::Color(40, 40, 50));
    _barBg.setOutlineColor(sf::Color(80, 80, 100));
    _barBg.setOutlineThickness(2.f);

    _barFill.setPosition(BAR_X, BAR_Y);
    _barFill.setSize({0.f, BAR_H});

    spawnPrey();
    buildHearts();
    buildGameOverX();
    updateBar();
}

// ─── update / event / render ─────────────────────────────────────────────────

void GameScene::update(float dt)
{
    if (_gameOver) return;
    updateSea(dt);
    cullDrownedPrey();
    updatePrey(dt);
    updateRedSphere(dt);
}

void GameScene::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left)
        handleClick({(float)event.mouseButton.x, (float)event.mouseButton.y});

    if (_gameOver &&
        event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::R)
        reset();
}

void GameScene::render()
{
    sf::RenderWindow& win = _game.getWindow();

    win.draw(_sky);
    win.draw(_water);
    for (const auto& p : _prey) {
        if (p.alive && p.entity)
            p.entity->render(win);
    }
    if (_chicken)
        _chicken->render(win);
    win.draw(_barBg);
    win.draw(_barFill);
    for (const auto& h : _hearts)
        win.draw(h);
    if (_gameOver) {
        win.draw(_gameOverX[0]);
        win.draw(_gameOverX[1]);
    }
}
} // namespace GraphLib