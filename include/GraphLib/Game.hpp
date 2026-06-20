/*
** EPITECH PROJECT, 2026
** Game.hpp
** File description:
** Game class
*/

#pragma once

#include "GraphLib/Scene.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <string>

namespace GraphLib {

    class AScene;

    typedef enum currentScene {
        MAIN_MENU = 0,
        GAME_SCENE = 1,
    } currentScene_t;

    class Game {
        public:
            Game();
            ~Game();
            void loadTexture(const std::string& name, const std::string& filename);
            sf::Texture& getTexture(const std::string& name);
            void setCurrentScene(currentScene_t scene);
            currentScene_t getCurrentScene() const;
            void loadScene(currentScene_t scene, std::unique_ptr<AScene> scenePtr);
            void run();

        private:
            std::unordered_map<std::string, sf::Texture> _textures;
            std::unordered_map<currentScene_t, std::unique_ptr<AScene>> _scenes;
            currentScene_t _currentScene;
            sf::RenderWindow _window;
    };
}
