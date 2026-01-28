#pragma once

// Standard Library
#include <vector>
#include <queue>

// Third-Party
#include <SFML/Graphics.hpp>

// Project
#include "ai/BaseStrategy.hpp"
#include "ai/GridHeatMap.hpp"

class AdvancedStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    void render(sf::RenderWindow& window) const override;

private:
    static constexpr int MAX_PATH_LENGTH = 200;  
    static constexpr int PATHFINDING_TIMEOUT_MS = 100;  

    // Member variables
    GridHeatMap gridHeatMap;
    std::vector<Direction> lastPath;
    sf::Clock pathUpdateClock;
    sf::Clock updateClock;

    // Helper functions
    void updateHeatMapVisualization(const Snake& snake, const sf::Vector2i& food);
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food);
    std::vector<Direction> findPathToFood(const Snake& snake, const sf::Vector2i& food);
    float calculateSimpleScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    bool isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const;
    std::vector<std::vector<Direction>> findAlternativePaths(
        const Snake& snake, const sf::Vector2i& food);
    std::vector<Direction> quickPathFind(
        const Position& start, const Position& goal, const Snake& snake) const;

    bool isValidPosition(const sf::Vector2i& pos) const {
        return pos.x >= 0 && pos.x < GameConfig::GRID_WIDTH && 
               pos.y >= 0 && pos.y < GameConfig::GRID_HEIGHT;
    }

    using BaseStrategy::getManhattanDistance;
    using BaseStrategy::isPositionBlocked;
    using BaseStrategy::countAccessibleSpace;
    using BaseStrategy::isMoveSafe;
};