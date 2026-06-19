/*
** EPITECH PROJECT, 2026
** Game.cpp
** File description:
** Game class methodes
*/

#include "GraphLib/Game.hpp"

namespace GraphLib {
    Game::Game() : _currentScene(MAIN_MENU), _window(sf::VideoMode(800, 600), "GraphLib Game") {}
    Game::~Game() {}

    void Game::loadTexture(const std::string& name, const std::string& filename) {
        sf::Texture texture;
        if (!texture.loadFromFile(filename)) {
            throw std::runtime_error("Failed to load texture: " + filename);
        }
        _textures[name] = std::move(texture);
    }

    sf::Texture& Game::getTexture(const std::string& name) {
        auto it = _textures.find(name);
        if (it == _textures.end()) {
            throw std::runtime_error("Texture not found: " + name);
        }
        return it->second;
    }

    void Game::setCurrentScene(currentScene_t scene) {
        _currentScene = scene;
    }

    currentScene_t Game::getCurrentScene() const {
        return _currentScene;
    }

    void Game::loadScene(currentScene_t scene, std::unique_ptr<Scene> scenePtr) {
        _scenes[scene] = std::move(scenePtr);
    }

    void Game::run() {
        while (_window.isOpen()) {
            sf::Event event;
            while (_window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    _window.close();
                }
                if (_scenes.find(_currentScene) != _scenes.end()) {
                    _scenes[_currentScene]->handleEvent(event);
                }
            }

            _window.clear();
            if (_scenes.find(_currentScene) != _scenes.end()) {
                _scenes[_currentScene]->update();
                _scenes[_currentScene]->render();
            }
            _window.display();
        }
    }
}
