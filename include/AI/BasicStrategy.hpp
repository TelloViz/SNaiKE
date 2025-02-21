#pragma once
#include "BaseStrategy.hpp"

class BasicStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;

private:
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
};