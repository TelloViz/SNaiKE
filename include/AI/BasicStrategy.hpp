#pragma once
#include "BaseStrategy.hpp"
#include "AI/GridHeatMap.hpp"
#include <SFML/Graphics.hpp>

class BasicStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    void render(sf::RenderWindow& window) const override {
        gridHeatMap.render(window, sf::Vector2f(GameConfig::CELL_SIZE, GameConfig::CELL_SIZE));
    }

private:
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    GridHeatMap gridHeatMap;  // Add this member variable
};