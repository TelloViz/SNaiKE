#pragma once
#include "AI/BaseStrategy.hpp"
#include <vector>
#include <map>

class ManhattanStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override {}
    void render(sf::RenderWindow& window) const override;
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food) const;
    void toggleHeatMap() override;  // Add this line

private:
    mutable std::map<Position, float> heatValues;
};