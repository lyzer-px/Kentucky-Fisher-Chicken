/*
** EPITECH PROJECT, 2026
** Entity.hpp
** File description:
** Entity class
*/


#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

namespace GraphLib {
    class Animation {
        public:
            Animation(
                std::size_t frameCount,
                float frameDuration,
                sf::Vector2i frameSize,
                std::size_t columns,
                std::size_t rows,
                sf::Vector2i startFrame = sf::Vector2i(0, 0),
                std::size_t currentFrame = 0
            );
            ~Animation();
            void setFrameCount(std::size_t frameCount);
            void setFrameDuration(float frameDuration);
            void setFrameSize(const sf::Vector2i& frameSize);
            void setColumns(std::size_t columns);
            void setRows(std::size_t rows);
            std::size_t getFrameCount() const;
            float getFrameDuration() const;
            sf::Vector2i getFrameSize() const;
            std::size_t getColumns() const;
            std::size_t getRows() const;
            sf::IntRect getCurrentFrameRect() const;
            void update(sf::Sprite& sprite, float deltaTime);

        private:
            sf::Vector2i _frameSize;
            sf::Vector2i _startFrame;
            std::size_t _currentFrame;
            std::size_t _frameCount;
            std::size_t _columns;
            std::size_t _rows;
            float _frameDuration;
            float _elapsedTime = 0.0f;
    };

    class Entity {
        public:
            Entity(sf::Texture& texture, std::string name);
            Entity(sf::Texture& texture, std::string name, Animation& animation);
            ~Entity();
            bool hasAnimation() const;
            void setAnimation(std::unique_ptr<Animation> animation);
            void setPosition(const sf::Vector2f& position);
            void setSize(const sf::Vector2f& size);
            void setVelocity(const sf::Vector2f& velocity);
            void setColor(const sf::Color& color);
            void render(sf::RenderWindow& window);
            void update(float deltaTime);
            const std::string &getName() const;

        private:
            std::string _entityName;
            std::unique_ptr<Animation> _animation;
            sf::Sprite _sprite;
            sf::Vector2f _position;
            sf::Vector2f _size;
            sf::Vector2f _velocity;
            sf::Color _color;
            bool _isVisible;
    };
}
