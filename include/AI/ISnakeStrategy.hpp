#pragma once
#include "Game/Direction.hpp"
#include "Game/Position.hpp"
#include "Snake.hpp"
#include <SFML/System/Vector2.hpp>

class ISnakeStrategy {
public:
    virtual ~ISnakeStrategy() = default;
    virtual Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) = 0;
    virtual void update() = 0; // For updating internal state if needed
};