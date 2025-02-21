#pragma once
#include "BaseStrategy.hpp"

class BasicStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    const HeatMap& getHeatMap() const override { return heatMap; }

private:
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
};