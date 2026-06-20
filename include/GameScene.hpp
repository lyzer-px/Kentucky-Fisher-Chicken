/*
** EPITECH PROJECT, 2026
** GameScene.hpp
** File description:
** GameScene class - diving game
*/

#pragma once

#include "GraphLib/Scene.hpp"
#include "GraphLib/Game.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <array>

namespace GraphLib {

    struct BlueSphere {
        sf::CircleShape shape;
        sf::Vector2f    velocity;
        bool            alive = true;
    };

    enum class RedState {
        HOVERING,
        DIVING,
        RETURNING,
    };

    class GameScene : public AScene {
    public:
        GameScene(Game& game);
        ~GameScene() = default;

        void render() override;
        void update(float deltaTime) override;
        void handleEvent(const sf::Event& event) override;

    private:
        // ── fixed layout ──────────────────────────────────────────────
        static constexpr float WATER_Y        = 300.f;
        static constexpr float MAX_DIVE_DEPTH = 120.f;
        static constexpr float SPHERE_RADIUS  = 18.f;
        static constexpr float RED_RADIUS     = 22.f;
        static constexpr float HOVER_Y        = 250.f;
        static constexpr float DIVE_SPEED     = 280.f;
        static constexpr float RETURN_SPEED   = 220.f;
        static constexpr int   MAX_LIVES      = 3;
        static constexpr int   CATCHES_TO_WIN = 10;

        // stomach bar geometry — top-right, small
        static constexpr float BAR_W      = 76.f;   // 760 / 10
        static constexpr float BAR_H      = 8.f;    // 22 / ~3, feels right at this scale
        static constexpr float BAR_X      = 800.f - BAR_W - 12.f;
        static constexpr float BAR_Y      = 12.f;
        static constexpr float BAR_RADIUS = 2.f;

        // ── per-level state ───────────────────────────────────────────
        int   _level;           // current level (starts at 1)
        float _blueSpeed;       // increases per level
        int   _sphereCount;     // increases per level

        // ── red sphere ────────────────────────────────────────────────
        sf::CircleShape _redSphere;
        sf::Vector2f    _redPos;
        sf::Vector2f    _redTarget;
        RedState        _redState;
        float           _bobTimer;

        // ── blue spheres ──────────────────────────────────────────────
        std::vector<BlueSphere> _blueSpheres;

        // ── game state ────────────────────────────────────────────────
        int  _catchesThisLevel; // 0–10, drives bar fill
        int  _lives;
        bool _gameOver;

        // ── UI shapes ─────────────────────────────────────────────────
        // Stomach bar
        sf::RectangleShape _barBg;      // dark background track
        sf::RectangleShape _barFill;    // coloured fill

        // Lives — 3 heart shapes
        std::array<sf::ConvexShape, MAX_LIVES> _hearts;

        // Game-over X
        std::array<sf::RectangleShape, 2> _gameOverX;

        // Background
        sf::RectangleShape _sky;
        sf::RectangleShape _water;

        // ── helpers ───────────────────────────────────────────────────
        BlueSphere      makeBlueSphere() const;
        sf::ConvexShape makeHeart(sf::Vector2f center, float size, sf::Color color) const;
        void            spawnBlueSpheres();
        void            updateBlueSpheres(float dt);
        void            updateRedSphere(float dt);
        void            handleClick(sf::Vector2f pos);
        int             findClickedSphere(sf::Vector2f pos) const;
        void            buildHearts();
        void            buildGameOverX();
        void            updateBar();        // recompute fill width + colour
        void            levelUp();
        void            reset();
    };
}