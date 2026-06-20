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

    // ── circle types ──────────────────────────────────────────────────────────
    enum class SphereType {
        SMALL  = 0,  // radius 10, fill 1,  weight 60
        MEDIUM = 1,  // radius 16, fill 3,  weight 25
        LARGE  = 2,  // radius 24, fill 6,  weight 12
        RARE   = 3,  // radius 10, fill 10, weight 3
    };

    struct SphereTypeInfo {
        float       radius;
        int         fill;       // hunger points out of 10 to fill per catch
        int         weight;     // spawn weight (higher = more common)
        sf::Color   color;
        sf::Color   outline;
        const char* textureName;
    };

    // static inline const std::array<SphereTypeInfo, 4> SPHERE_TYPES = {{
    //     { 10.f, 1,  60, sf::Color(100, 160, 255), sf::Color(40,  80, 180) }, // small  blue
    //     { 16.f, 3,  25, sf::Color(255, 160,  40), sf::Color(180,  90,  0) }, // medium orange
    //     { 24.f, 6,  12, sf::Color(80,  210,  80), sf::Color(20,  120, 20) }, // large  green
    //     { 10.f, 10,  3, sf::Color(200,  60, 220), sf::Color(100,  10, 120) }, // rare   purple
    // }};

    static inline const std::array<SphereTypeInfo, 4> SPHERE_TYPES = {{
        { 10.f, 1,  60, sf::Color(100, 160, 255), sf::Color(40,  80, 180), "fish_blue"   },
        { 16.f, 3,  25, sf::Color(255, 160,  40), sf::Color(180,  90,  0), "fish_orange" },
        { 24.f, 6,  12, sf::Color(80,  210,  80), sf::Color(20,  120, 20), "fish_green"  },
        { 10.f, 10,  3, sf::Color(230,  80, 180), sf::Color(140,  20, 90), "fish_pink"   },
    }};

    struct Prey {
        std::unique_ptr<Entity> entity;
        sf::Vector2f    velocity;
        float           angle       = 0.f;   // current heading in radians
        float           targetAngle = 0.f;   // heading we're steering toward
        float           wanderTimer = 0.f;   // countdown to next direction change
        float           speed       = 0.f;   // scalar speed, kept constant
        SphereType      type;
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
        static constexpr float BASE_WATER_Y   = 300.f; // default sea surface
        static constexpr float SEA_MIN_Y      = 180.f; // highest the sea can reach
        static constexpr float SEA_MAX_Y      = 420.f; // lowest the sea can reach
        static constexpr float SEA_SHIFT_MIN  = 40.f;  // min jump per shift
        static constexpr float SEA_SHIFT_MAX  = 120.f; // max jump per shift
        static constexpr float SEA_ANIM_SPEED = 90.f;  // px/s the surface moves
        static constexpr float SEA_SHIFT_INTERVAL_MIN = 4.f;  // seconds between shifts
        static constexpr float SEA_SHIFT_INTERVAL_MAX = 9.f;

        static constexpr float MAX_DIVE_DEPTH = 120.f; // relative to current waterY
        static constexpr float RED_RADIUS     = 22.f;
        static constexpr float HOVER_OFFSET   = 50.f;  // px above current waterY
        static constexpr float DIVE_SPEED     = 280.f;
        static constexpr float RETURN_SPEED   = 220.f;
        static constexpr int   MAX_LIVES      = 3;
        static constexpr int   CATCHES_TO_WIN = 10;    // hunger points, not catch count

        // stomach bar geometry — top-right, small
        static constexpr float BAR_W = 76.f;
        static constexpr float BAR_H = 8.f;
        static constexpr float BAR_X = 800.f - BAR_W - 12.f;
        static constexpr float BAR_Y = 12.f;

        // ── sea state ─────────────────────────────────────────────────
        float _waterY;          // current rendered surface Y
        float _targetWaterY;    // where surface is heading
        float _seaShiftTimer;   // countdown to next shift

        // ── per-level state ───────────────────────────────────────────
        int   _level;
        float _preySpeed;
        int   _preyCount;

        // ── red sphere ────────────────────────────────────────────────
        std::unique_ptr<Entity> _chicken;
        sf::CircleShape _redSphere;
        sf::Vector2f    _redPos;
        sf::Vector2f    _redTarget;
        RedState        _redState;
        float           _bobTimer;

        // ── prey ──────────────────────────────────────────────────────
        std::vector<Prey> _prey;

        // ── game state ────────────────────────────────────────────────
        float _hungerFill;      // 0.0 – 1.0
        int   _lives;
        bool  _gameOver;

        // ── UI shapes ─────────────────────────────────────────────────
        sf::RectangleShape _barBg;
        sf::RectangleShape _barFill;
        std::array<sf::ConvexShape, MAX_LIVES> _hearts;
        std::array<sf::RectangleShape, 2>      _gameOverX;

        // Background
        sf::RectangleShape _sky;
        sf::RectangleShape _water;

        // ── helpers ───────────────────────────────────────────────────
        Prey            makePrey() const;
        SphereType      pickType() const;
        sf::ConvexShape makeHeart(sf::Vector2f center, float size, sf::Color color) const;
        void            spawnPrey();
        void            updatePrey(float dt);
        void            cullDrownedPrey();   // remove prey above waterline after sea drops
        void            updateSea(float dt);
        void            updateRedSphere(float dt);
        void            handleClick(sf::Vector2f pos);
        int             findClickedPrey(sf::Vector2f pos) const;
        void            buildHearts();
        void            buildGameOverX();
        void            updateBar();
        void            updateWaterShape();
        void            levelUp();
        void            reset();
        int             targetPreyCount() const; // base count + bonus for high sea
    };
}