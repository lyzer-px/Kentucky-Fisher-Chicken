/*
** EPITECH PROJECT, 2026
** Entity.cpp
** File description:
** Entity class methodes
*/

#include "GraphLib/Entity.hpp"
#include <string>

namespace GraphLib {

    Animation::Animation(
        std::size_t frameCount,
        float frameDuration,
        sf::Vector2i frameSize,
        std::size_t columns,
        std::size_t rows,
        sf::Vector2i startFrame,
        std::size_t currentFrame
    ) : _frameSize(frameSize),
    _startFrame(startFrame), 
    _currentFrame(currentFrame),
    _frameCount(frameCount),
    _columns(columns), _rows(rows),
    _frameDuration(frameDuration) {}

    Animation::~Animation() {}

    void Animation::setFrameCount(std::size_t frameCount) {
        _frameCount = frameCount;
    }

    void Animation::setFrameDuration(float frameDuration) {
        _frameDuration = frameDuration;
    }

    void Animation::setFrameSize(const sf::Vector2i& frameSize) {
        _frameSize = frameSize;
    }

    void Animation::setColumns(std::size_t columns) {
        _columns = columns;
    }

    void Animation::setRows(std::size_t rows) {
        _rows = rows;
    }

    std::size_t Animation::getFrameCount() const {
        return _frameCount;
    }

    float Animation::getFrameDuration() const {
        return _frameDuration;
    }

    sf::Vector2i Animation::getFrameSize() const {
        return _frameSize;
    }

    std::size_t Animation::getColumns() const {
        return _columns;
    }

    std::size_t Animation::getRows() const {
        return _rows;
    }

    sf::IntRect Animation::getCurrentFrameRect() const {
        std::size_t column = _currentFrame % _columns;
        std::size_t row = _currentFrame / _columns;
        return sf::IntRect(_startFrame.x + column * _frameSize.x, _startFrame.y + row * _frameSize.y, _frameSize.x, _frameSize.y);
    }

    void Animation::update(sf::Sprite& sprite, float deltaTime) {
        _elapsedTime += deltaTime;

        if (_elapsedTime >= _frameDuration) {
            _currentFrame = (_currentFrame + 1) % _frameCount;
            sprite.setTextureRect(getCurrentFrameRect());
            _elapsedTime = 0.0f;
        }
    }


    Entity::Entity(sf::Texture& texture, std::string name) : _entityName(name), _sprite(texture), _position(0, 0), _size(0, 0), _velocity(0, 0), _color(sf::Color::White), _isVisible(true) {}

    Entity::Entity(sf::Texture& texture, std::string name, Animation& animation) : _entityName(name), _sprite(texture), _position(0, 0), _size(0, 0), _velocity(0, 0), _color(sf::Color::White), _isVisible(true) {
        setAnimation(std::make_unique<Animation>(animation));
    }

    Entity::~Entity() {}

    bool Entity::hasAnimation() const {
        return _animation != nullptr;
    }

    void Entity::setAnimation(std::unique_ptr<Animation> animation) {
        _animation = std::move(animation);
        _sprite.setTextureRect(_animation->getCurrentFrameRect());
    }

    void Entity::setPosition(const sf::Vector2f& position) {
        _position = position;
        _sprite.setPosition(_position);
    }

    void Entity::setSize(const sf::Vector2f& size) {
        _size = size;
        _sprite.setScale(_size.x / _sprite.getTextureRect().width, _size.y / _sprite.getTextureRect().height);
    }

    void Entity::setVelocity(const sf::Vector2f& velocity) {
        _velocity = velocity;
    }

    void Entity::setColor(const sf::Color& color) {
        _color = color;
        _sprite.setColor(_color);
    }

    void Entity::render(sf::RenderWindow& window) {
        if (_isVisible) {
            window.draw(_sprite);
        }
    }

    void Entity::update(float deltaTime) {
        _position += _velocity * deltaTime;
        _sprite.setPosition(_position);
        if (_animation) {
            _animation->update(_sprite, deltaTime);
        }
    }

    const std::string &Entity::getName() const {
        return _entityName;
    }
}
