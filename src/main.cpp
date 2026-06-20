/*
** EPITECH PROJECT, 2026
** cloned
** File description:
** 
*/

#include <exception>
#include <iostream>
#include "GameScene.hpp"

int main(void)
{
    try {
        GraphLib::Game g;
        g.loadScene(GraphLib::GAME_SCENE, std::make_unique<GraphLib::GameScene>(g));
        g.setCurrentScene(GraphLib::GAME_SCENE);
        g.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

}
