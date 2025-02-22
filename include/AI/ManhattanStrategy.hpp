#pragma once
#include "AI/BaseStrategy.hpp"

class ManhattanStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override {}  // Empty implementation since Manhattan doesn't need updates
    void render(sf::RenderWindow& window) const override {}  // Empty implementation since Manhattan doesn't visualize
};