/*
** EPITECH PROJECT, 2026
** MainMenu.cpp
** File description:
** Main menu scene logic
*/

#include "MainMenu.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

static constexpr float PI = 3.14159265f;

namespace GraphLib {

static float randf(float lo, float hi) {
    return lo + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

// ─── fish ────────────────────────────────────────────────────────────────────

MenuFish MainMenu::makeFish() const
{
    MenuFish f;
    float r = randf(6.f, 18.f);
    f.shape.setRadius(r);
    f.shape.setOrigin(r, r);

    // Random muted underwater colour
    sf::Uint8 rv = static_cast<sf::Uint8>(randf(40, 160));
    sf::Uint8 gv = static_cast<sf::Uint8>(randf(80, 180));
    sf::Uint8 bv = static_cast<sf::Uint8>(randf(160, 255));
    f.shape.setFillColor(sf::Color(rv, gv, bv, 200));
    f.shape.setOutlineColor(sf::Color(rv / 2, gv / 2, bv / 2, 180));
    f.shape.setOutlineThickness(1.5f);

    float margin = r + 10.f;
    f.shape.setPosition(randf(margin, W - margin), randf(_waterY + margin, H - margin));

    f.angle       = randf(0.f, 2.f * PI);
    f.targetAngle = f.angle;
    f.speed       = randf(30.f, 70.f);
    f.wanderTimer = randf(1.f, 3.f);
    return f;
}

void MainMenu::spawnFish(int count)
{
    _fish.clear();
    for (int i = 0; i < count; ++i)
        _fish.push_back(makeFish());
}

void MainMenu::updateFish(float dt)
{
    static constexpr float TURN = 2.f;
    for (auto& f : _fish) {
        f.wanderTimer -= dt;
        if (f.wanderTimer <= 0.f) {
            f.targetAngle += randf(-PI * 0.6f, PI * 0.6f);
            f.wanderTimer  = randf(1.f, 3.f);
        }

        float diff = f.targetAngle - f.angle;
        while (diff >  PI) diff -= 2.f * PI;
        while (diff < -PI) diff += 2.f * PI;
        float maxTurn = TURN * dt;
        f.angle += std::clamp(diff, -maxTurn, maxTurn);

        float r = f.shape.getRadius();
        sf::Vector2f pos = f.shape.getPosition();
        pos += sf::Vector2f{std::cos(f.angle) * f.speed, std::sin(f.angle) * f.speed} * dt;

        if (pos.x - r < 0.f)    { pos.x = r;       f.angle = PI - f.angle; f.targetAngle = f.angle; }
        if (pos.x + r > W)       { pos.x = W - r;   f.angle = PI - f.angle; f.targetAngle = f.angle; }
        if (pos.y - r < _waterY) { pos.y = _waterY + r; f.angle = -f.angle; f.targetAngle = f.angle; }
        if (pos.y + r > H)       { pos.y = H - r;   f.angle = -f.angle;     f.targetAngle = f.angle; }

        f.shape.setPosition(pos);
    }
}

// ─── sea ─────────────────────────────────────────────────────────────────────

void MainMenu::updateSea(float dt)
{
    if (std::abs(_waterY - _targetWaterY) > 1.f) {
        float dir  = (_targetWaterY > _waterY) ? 1.f : -1.f;
        float step = SEA_ANIM_SPEED * dt;
        _waterY = (step >= std::abs(_targetWaterY - _waterY))
                ? _targetWaterY
                : _waterY + dir * step;
        _water.setPosition(0.f, _waterY);
        _water.setSize({W, H - _waterY});
    }

    _seaShiftTimer -= dt;
    if (_seaShiftTimer <= 0.f) {
        float shift     = randf(SEA_SHIFT_MIN, SEA_SHIFT_MAX);
        bool  goUp      = randf(0.f, 1.f) < 0.5f;
        _targetWaterY   = std::clamp(_targetWaterY + (goUp ? -shift : shift), SEA_MIN_Y, SEA_MAX_Y);
        _seaShiftTimer  = randf(SEA_SHIFT_INTERVAL_MIN, SEA_SHIFT_INTERVAL_MAX);
    }

    // Push fish back under the surface if sea rose
    for (auto& f : _fish) {
        float r = f.shape.getRadius();
        sf::Vector2f pos = f.shape.getPosition();
        if (pos.y - r < _waterY) {
            pos.y = _waterY + r;
            f.shape.setPosition(pos);
            f.angle = -std::abs(f.angle); // send downward
            f.targetAngle = f.angle;
        }
    }
}

// ─── button ──────────────────────────────────────────────────────────────────

void MainMenu::buildButton()
{
    _btnBg.setSize({BTN_W, BTN_H});
    _btnBg.setPosition(BTN_X, BTN_Y);
    _btnBg.setOutlineThickness(2.f);

    _btnHighlight.setSize({BTN_W - 4.f, 4.f});
    _btnHighlight.setPosition(BTN_X + 2.f, BTN_Y + 2.f);
    _btnHighlight.setFillColor(sf::Color(255, 255, 255, 60));

    // Play triangle centered in button
    _playTriangle.setPointCount(3);
    float cx = BTN_X + BTN_W / 2.f + 4.f;
    float cy = BTN_Y + BTN_H / 2.f;
    float s  = 14.f;
    _playTriangle.setPoint(0, {cx - s * 0.6f, cy - s});
    _playTriangle.setPoint(1, {cx - s * 0.6f, cy + s});
    _playTriangle.setPoint(2, {cx + s,         cy});
    _playTriangle.setFillColor(sf::Color::White);
}

void MainMenu::updateButton(sf::Vector2f mouse)
{
    _btnHovered = _btnBg.getGlobalBounds().contains(mouse);

    if (_btnHovered) {
        _btnBg.setFillColor(sf::Color(60, 160, 80));
        _btnBg.setOutlineColor(sf::Color(120, 220, 140));
    } else {
        _btnBg.setFillColor(sf::Color(40, 120, 60));
        _btnBg.setOutlineColor(sf::Color(80, 180, 100));
    }
}

// ─── constructor ─────────────────────────────────────────────────────────────

MainMenu::MainMenu(Game& game)
    : AScene(game),
      _waterY(BASE_WATER_Y),
      _targetWaterY(BASE_WATER_Y),
      _seaShiftTimer(5.f),
      _bobTimer(0.f),
      _btnHovered(false)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    _sky.setSize({W, H});
    _sky.setPosition(0.f, 0.f);
    _sky.setFillColor(sf::Color(135, 206, 235));

    _water.setPosition(0.f, _waterY);
    _water.setSize({W, H - _waterY});
    _water.setFillColor(sf::Color(30, 80, 160, 210));

    // Red sphere
    _redSphere.setRadius(22.f);
    _redSphere.setOrigin(22.f, 22.f);
    _redSphere.setFillColor(sf::Color(220, 40, 40));
    _redSphere.setOutlineColor(sf::Color(140, 10, 10));
    _redSphere.setOutlineThickness(2.f);
    _redSphere.setPosition(W / 2.f, _waterY - 50.f);

    spawnFish(8);
    buildButton();
}

// ─── update / event / render ─────────────────────────────────────────────────

void MainMenu::update(float dt)
{
    updateSea(dt);
    updateFish(dt);

    _bobTimer += dt;
    _redSphere.setPosition(W / 2.f, (_waterY - 50.f) + std::sin(_bobTimer * 2.f) * 6.f);
}

void MainMenu::handleEvent(const sf::Event& event)
{
    if (event.type == sf::Event::MouseMoved)
        updateButton({(float)event.mouseMove.x, (float)event.mouseMove.y});

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f click{(float)event.mouseButton.x, (float)event.mouseButton.y};
        if (_btnBg.getGlobalBounds().contains(click))
            _game.setCurrentScene(GAME_SCENE);
    }
}

void MainMenu::render()
{
    sf::RenderWindow& win = _game.getWindow();
    win.draw(_sky);
    win.draw(_water);

    for (const auto& f : _fish)
        win.draw(f.shape);

    win.draw(_redSphere);
    win.draw(_btnBg);
    win.draw(_btnHighlight);
    win.draw(_playTriangle);
}

} // namespace GraphLib