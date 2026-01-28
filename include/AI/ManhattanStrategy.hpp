#pragma once

// Standard Library Includes
#include <vector>
#include <map>

// Project
#include "ai/BaseStrategy.hpp"


class ManhattanStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override {}
    void render(sf::RenderWindow& window) const override;
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food) const;
    void toggleHeatMap() override; 

private:
    mutable std::map<Position, float> heatValues;
};