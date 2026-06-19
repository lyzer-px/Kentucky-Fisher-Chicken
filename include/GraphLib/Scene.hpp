/*
** EPITECH PROJECT, 2026
** Scene.hpp
** File description:
** Scene class
*/

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

namespace GraphLib {
    class Game;
    class Entity;

    class AScene {
        public:
            AScene(Game& game);
            ~AScene();
            void addEntity(std::unique_ptr<Entity> entity);
            Entity* getEntity(const std::string& name) const;

            /// \brief              render is for rendering the scene, it will be called by the Game class every frame
            ///
            /// \note               the Game class is stored in the _game attribute, you can use it to get the window and render your scene
            virtual void render() = 0;

            /// \brief              update is for updating the scene, it will be called by the Game class every frame
            ///
            /// \param deltaTime    The time elapsed since the last frame, in seconds
            virtual void update(float deltaTime) = 0;

            /// \brief              handleEvent is for handling user inputs, appart from the close event, which is handled by the Game class itself
            ///
            /// \param event        the event to handle, the Game class will call this method for every event that is not a close event
            ///
            /// \note               there is no need to do the pollEvent, as the Game class will handle it and call this method for every event
            virtual void handleEvent(const sf::Event& event) = 0;
        protected:
            Game& _game;
            std::vector<std::unique_ptr<Entity>> _entities;
    };
}
