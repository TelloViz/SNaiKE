#pragma once
#include "AI/BaseStrategy.hpp"
#include "Snake.hpp"
#include <SFML/Graphics.hpp>

class FloodFillStrategy : public BaseStrategy {
public:
    FloodFillStrategy(const Snake& snakeRef) : snake(snakeRef), showPathArrows(false) {}
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override {}
    void render(sf::RenderWindow& window) const override;
    void toggleHeatMap() override { globalShowHeatMap = !globalShowHeatMap; }
    void togglePathArrows() { showPathArrows = !showPathArrows; }
    bool isPathArrowsEnabled() const { return showPathArrows; }

private:
    int floodFill(const Snake& snake, const sf::Vector2i& pos) const;
    bool isAreaLargeEnough(const Snake& snake, const sf::Vector2i& nextPos) const;
    void renderArrows(sf::RenderWindow& window, const sf::Vector2i& current) const;
    Direction findBestMove(const sf::Vector2i& pos) const;
    const Snake& snake;
    mutable std::vector<std::vector<int>> heatMap;
    bool showPathArrows;
    sf::Vector2i food;
};