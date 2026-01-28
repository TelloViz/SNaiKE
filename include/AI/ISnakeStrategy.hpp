#pragma once

// Third-Party Includes
#include <SFML/System/Vector2.hpp>

// Project Includes
#include "game/Direction.hpp"
#include "game/Position.hpp"
#include "Snake.hpp"
#include "ai/HeatMap.hpp"

class ISnakeStrategy {
public:
    virtual ~ISnakeStrategy() = default;
    virtual Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) = 0;
    virtual void update() = 0;
    virtual const HeatMap& getHeatMap() const = 0;

protected:
    HeatMap heatMap;
};