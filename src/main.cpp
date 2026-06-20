/*
** EPITECH PROJECT, 2026
** main.cpp
** File description:
** Entry point
*/

#include "GraphLib/Game.hpp"
#include "GameScene.hpp"
#include "MainMenu.hpp"
#include <exception>
#include <iostream>

int main(void)
{
    try {
        GraphLib::Game g;
        g.loadScene(GraphLib::MAIN_MENU,  std::make_unique<GraphLib::MainMenu>(g));
        g.loadScene(GraphLib::GAME_SCENE, std::make_unique<GraphLib::GameScene>(g));
        g.setCurrentScene(GraphLib::MAIN_MENU);
        g.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}