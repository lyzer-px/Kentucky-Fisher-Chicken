/*
** EPITECH PROJECT, 2026
** Scene.cpp
** File description:
** Scene class methodes
*/

#include "GraphLib/Scene.hpp"

namespace GraphLib {
    AScene::AScene(Game& game) : _game(game) {}

    void AScene::addEntity(std::unique_ptr<Entity> entity) {
        _entities.push_back(std::move(entity));
    }

    Entity* AScene::getEntity(const std::string& name) const {
        for (const auto& entity : _entities) {
            if (entity->getName() == name) {
                return entity.get();
            }
        }
        return nullptr;
    }
}
