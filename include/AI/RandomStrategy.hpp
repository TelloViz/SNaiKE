#pragma once
#include "BaseStrategy.hpp"

class RandomStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override {}
    const HeatMap& getHeatMap() const override { return heatMap; }
};