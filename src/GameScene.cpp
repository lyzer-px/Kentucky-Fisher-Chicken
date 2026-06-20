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

static constexpr float PI = 3.14159265f;

namespace GraphLib {

// ─── helpers ────────────────────────────────────────────────────────────────

static float randf(float lo, float hi) {
    return lo + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

// Linearly interpolate between two colours by t in [0,1]
static sf::Color lerpColor(sf::Color a, sf::Color b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return sf::Color(
        static_cast<sf::Uint8>(a.r + (b.r - a.r) * t),
        static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
        static_cast<sf::Uint8>(a.b + (b.b - a.b) * t)
    );
}

// ─── heart ──────────────────────────────────────────────────────────────────

sf::ConvexShape GameScene::makeHeart(sf::Vector2f center, float size, sf::Color color) const
{
    const int N = 40;
    sf::ConvexShape h;
    h.setPointCount(N);
    for (int i = 0; i < N; ++i) {
        float t = static_cast<float>(i) / N * 2.f * PI;
        float x =  size * 0.9f * std::pow(std::sin(t), 3.f);
        float y = -size * (0.8125f * std::cos(t)
                         - 0.3125f * std::cos(2.f * t)
                         - 0.125f  * std::cos(3.f * t)
                         - 0.0625f * std::cos(4.f * t));
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

// ─── game-over X ────────────────────────────────────────────────────────────

void GameScene::buildGameOverX()
{
    const float CX = 400.f, CY = 300.f, LEN = 80.f, TH = 18.f;
    for (int i = 0; i < 2; ++i) {
        _gameOverX[i].setSize({LEN * 2.f, TH});
        _gameOverX[i].setOrigin(LEN, TH / 2.f);
        _gameOverX[i].setPosition(CX, CY);
        _gameOverX[i].setRotation(i == 0 ? 45.f : -45.f);
        _gameOverX[i].setFillColor(sf::Color(220, 40, 40));
    }
}

// ─── stomach bar ────────────────────────────────────────────────────────────

void GameScene::updateBar()
{
    float t = static_cast<float>(_catchesThisLevel) / static_cast<float>(CATCHES_TO_WIN);
    t = std::clamp(t, 0.f, 1.f);

    // Colour: red → orange → green
    sf::Color barColor;
    if (t < 0.5f)
        barColor = lerpColor(sf::Color(210, 40, 40), sf::Color(230, 150, 20), t * 2.f);
    else
        barColor = lerpColor(sf::Color(230, 150, 20), sf::Color(60, 200, 80), (t - 0.5f) * 2.f);

    float fillW = BAR_W * t;
    _barFill.setSize({fillW, BAR_H});
    _barFill.setFillColor(barColor);
}

// ─── blue spheres ────────────────────────────────────────────────────────────

BlueSphere GameScene::makeBlueSphere() const
{
    BlueSphere bs;
    bs.shape.setRadius(SPHERE_RADIUS);
    bs.shape.setOrigin(SPHERE_RADIUS, SPHERE_RADIUS);
    bs.shape.setFillColor(sf::Color(60, 140, 255));
    bs.shape.setOutlineColor(sf::Color(20, 60, 180));
    bs.shape.setOutlineThickness(2.f);

    float x = randf(SPHERE_RADIUS + 10.f, 800.f - SPHERE_RADIUS - 10.f);
    float y = randf(WATER_Y + SPHERE_RADIUS + 10.f, 600.f - SPHERE_RADIUS - 10.f);
    bs.shape.setPosition(x, y);

    float angle = randf(0.f, 2.f * PI);
    float speed = randf(_blueSpeed * 0.5f, _blueSpeed * 1.5f);
    bs.velocity  = {std::cos(angle) * speed, std::sin(angle) * speed};
    return bs;
}

void GameScene::spawnBlueSpheres()
{
    _blueSpheres.clear();
    for (int i = 0; i < _sphereCount; ++i)
        _blueSpheres.push_back(makeBlueSphere());
}

void GameScene::updateBlueSpheres(float dt)
{
    const float W = 800.f, H = 600.f;
    for (auto& bs : _blueSpheres) {
        if (!bs.alive) continue;
        sf::Vector2f pos = bs.shape.getPosition();
        pos += bs.velocity * dt;

        if (pos.x - SPHERE_RADIUS < 0.f)    { pos.x = SPHERE_RADIUS;            bs.velocity.x =  std::abs(bs.velocity.x); }
        if (pos.x + SPHERE_RADIUS > W)        { pos.x = W - SPHERE_RADIUS;        bs.velocity.x = -std::abs(bs.velocity.x); }
        if (pos.y - SPHERE_RADIUS < WATER_Y)  { pos.y = WATER_Y + SPHERE_RADIUS;  bs.velocity.y =  std::abs(bs.velocity.y); }
        if (pos.y + SPHERE_RADIUS > H)        { pos.y = H - SPHERE_RADIUS;        bs.velocity.y = -std::abs(bs.velocity.y); }

        bs.shape.setPosition(pos);

        if (randf(0.f, 1.f) < 0.008f) {
            float angle = randf(0.f, 2.f * PI);
            float speed = randf(_blueSpeed * 0.5f, _blueSpeed * 1.5f);
            bs.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        }
    }
}

// ─── red sphere ──────────────────────────────────────────────────────────────

void GameScene::updateRedSphere(float dt)
{
    if (_redState == RedState::HOVERING) {
        _bobTimer += dt;
        _redPos.y = HOVER_Y + std::sin(_bobTimer * 2.f) * 6.f;
        _redSphere.setPosition(_redPos);
        return;
    }

    sf::Vector2f dir  = _redTarget - _redPos;
    float        dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    float        spd  = (_redState == RedState::DIVING) ? DIVE_SPEED : RETURN_SPEED;

    if (dist < spd * dt) {
        _redPos = _redTarget;
        _redSphere.setPosition(_redPos);
        if (_redState == RedState::DIVING) {
            _redTarget = {_redPos.x, HOVER_Y};
            _redState  = RedState::RETURNING;
        } else {
            _redPos.x  = 400.f;
            _redTarget = _redPos;
            _redState  = RedState::HOVERING;
            _bobTimer  = 0.f;
        }
    } else {
        dir    /= dist;
        _redPos += dir * spd * dt;
        _redSphere.setPosition(_redPos);
    }
}

// ─── click handling ──────────────────────────────────────────────────────────

int GameScene::findClickedSphere(sf::Vector2f pos) const
{
    for (int i = 0; i < (int)_blueSpheres.size(); ++i) {
        if (!_blueSpheres[i].alive) continue;
        sf::Vector2f d = pos - _blueSpheres[i].shape.getPosition();
        if (std::sqrt(d.x * d.x + d.y * d.y) <= SPHERE_RADIUS + 6.f)
            return i;
    }
    return -1;
}

void GameScene::handleClick(sf::Vector2f pos)
{
    if (_gameOver || _redState != RedState::HOVERING) return;

    int idx = findClickedSphere(pos);
    if (idx == -1) return;

    sf::Vector2f target = _blueSpheres[idx].shape.getPosition();
    float        depth  = target.y - WATER_Y;

    _redTarget = target;
    _redState  = RedState::DIVING;

    if (depth <= MAX_DIVE_DEPTH) {
        _blueSpheres[idx].alive = false;
        _blueSpheres.push_back(makeBlueSphere());
        _catchesThisLevel++;

        if (_catchesThisLevel >= CATCHES_TO_WIN)
            levelUp();
        else
            updateBar();
    } else {
        _lives--;
        if (_lives <= 0) { _lives = 0; _gameOver = true; }
        buildHearts();
    }
}

// ─── level up ────────────────────────────────────────────────────────────────

void GameScene::levelUp()
{
    _level++;
    _catchesThisLevel = 0;
    _blueSpeed   = 60.f + (_level - 1) * 20.f;   // +20 px/s per level
    _sphereCount = 6    + (_level - 1) * 2;        // +2 spheres per level
    spawnBlueSpheres();
    updateBar();
}

// ─── reset ───────────────────────────────────────────────────────────────────

void GameScene::reset()
{
    _level            = 1;
    _catchesThisLevel = 0;
    _lives            = MAX_LIVES;
    _gameOver         = false;
    _blueSpeed        = 60.f;
    _sphereCount      = 6;
    _redPos           = {400.f, HOVER_Y};
    _redSphere.setPosition(_redPos);
    _redState         = RedState::HOVERING;
    _bobTimer         = 0.f;
    spawnBlueSpheres();
    buildHearts();
    updateBar();
}

// ─── constructor ─────────────────────────────────────────────────────────────

GameScene::GameScene(Game& game)
    : AScene(game),
      _level(1),
      _blueSpeed(60.f),
      _sphereCount(6),
      _redState(RedState::HOVERING),
      _bobTimer(0.f),
      _catchesThisLevel(0),
      _lives(MAX_LIVES),
      _gameOver(false)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Background
    _sky.setSize({800.f, WATER_Y});
    _sky.setPosition(0, 0);
    _sky.setFillColor(sf::Color(135, 206, 235));

    _water.setSize({800.f, 600.f - WATER_Y});
    _water.setPosition(0, WATER_Y);
    _water.setFillColor(sf::Color(30, 80, 160, 210));

    // Red sphere
    _redPos = {400.f, HOVER_Y};
    _redSphere.setRadius(RED_RADIUS);
    _redSphere.setOrigin(RED_RADIUS, RED_RADIUS);
    _redSphere.setFillColor(sf::Color(220, 40, 40));
    _redSphere.setOutlineColor(sf::Color(140, 10, 10));
    _redSphere.setOutlineThickness(2.f);
    _redSphere.setPosition(_redPos);

    // Stomach bar — background track
    _barBg.setSize({BAR_W, BAR_H});
    _barBg.setPosition(BAR_X, BAR_Y);
    _barBg.setFillColor(sf::Color(40, 40, 50));
    _barBg.setOutlineColor(sf::Color(80, 80, 100));
    _barBg.setOutlineThickness(2.f);

    // Stomach bar — fill (starts at 0 width)
    _barFill.setPosition(BAR_X, BAR_Y);
    _barFill.setSize({0.f, BAR_H});

    spawnBlueSpheres();
    buildHearts();
    buildGameOverX();
    updateBar();
}

// ─── update / event / render ─────────────────────────────────────────────────

void GameScene::update(float dt)
{
    if (_gameOver) return;
    updateBlueSpheres(dt);
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
    sf::RenderWindow& w = _game.getWindow();

    w.draw(_sky);
    w.draw(_water);

    for (const auto& bs : _blueSpheres)
        if (bs.alive) w.draw(bs.shape);

    w.draw(_redSphere);

    // Stomach bar
    w.draw(_barBg);
    w.draw(_barFill);

    // Hearts
    for (const auto& h : _hearts)
        w.draw(h);

    // Game-over X
    if (_gameOver) {
        w.draw(_gameOverX[0]);
        w.draw(_gameOverX[1]);
    }
}

} // namespace GraphLib