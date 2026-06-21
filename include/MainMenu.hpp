/*
** EPITECH PROJECT, 2026
** MainMenu.hpp
** File description:
** Main menu scene
*/

#pragma once

#include "GraphLib/Scene.hpp"
#include "GraphLib/Game.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

namespace GraphLib {

    // A decorative fish swimming in the menu background
    struct MenuFish {
        std::unique_ptr<Entity> entity;
        float           radius      = 0.f;
        sf::Vector2f    velocity;
        float           angle       = 0.f;
        float           targetAngle = 0.f;
        float           wanderTimer = 0.f;
        float           speed       = 0.f;
    };

    class MainMenu : public AScene {
    public:
        MainMenu(Game& game);
        ~MainMenu() = default;

        void render() override;
        void update(float deltaTime) override;
        void handleEvent(const sf::Event& event) override;

    private:
        static constexpr float W              = 800.f;
        static constexpr float H              = 600.f;
        static constexpr float BASE_WATER_Y   = 280.f;
        static constexpr float SEA_MIN_Y      = 160.f;
        static constexpr float SEA_MAX_Y      = 380.f;
        static constexpr float SEA_ANIM_SPEED = 70.f;
        static constexpr float SEA_SHIFT_MIN  = 40.f;
        static constexpr float SEA_SHIFT_MAX  = 100.f;
        static constexpr float SEA_SHIFT_INTERVAL_MIN = 4.f;
        static constexpr float SEA_SHIFT_INTERVAL_MAX = 9.f;

        // Button geometry
        static constexpr float BTN_W = 160.f;
        static constexpr float BTN_H = 50.f;
        static constexpr float BTN_X = (W - BTN_W) / 2.f;
        static constexpr float BTN_Y = H / 2.f + 40.f;

        // Sea
        float _waterY;
        float _targetWaterY;
        float _seaShiftTimer;

        // Background
        sf::RectangleShape _sky;
        sf::RectangleShape _water;

        // Red sphere bobbing on surface
        sf::CircleShape _redSphere;
        std::unique_ptr<Entity> _chicken;
        float           _bobTimer;

        // Decorative fish
        std::vector<MenuFish> _fish;

        // Play button
        sf::RectangleShape _btnBg;
        sf::RectangleShape _btnHighlight; // top-edge shine
        // Play triangle (drawn as ConvexShape)
        sf::ConvexShape    _playTriangle;

        // Hover state
        bool _btnHovered;

        // Helpers
        void updateSea(float dt);
        void updateFish(float dt);
        void updateButton(sf::Vector2f mousePos);
        void spawnFish(int count);
        MenuFish makeFish() const;
        void buildButton();
    };
}