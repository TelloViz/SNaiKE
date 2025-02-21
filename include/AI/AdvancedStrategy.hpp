#pragma once
#include "BaseStrategy.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "GridHeatMap.hpp"

class AdvancedStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    void render(sf::RenderWindow& window) const override {
        gridHeatMap.render(window, sf::Vector2f(GameConfig::CELL_SIZE, GameConfig::CELL_SIZE));
    }

private:
    sf::Vector2i lastPosition;
    std::vector<Direction> lastPath;
    sf::Clock pathfindClock;
    Position lastFoodPos;
    int stuckCount{0};
    std::vector<std::vector<float>> scoreCache;
    sf::Clock gameTimer;
    GridHeatMap gridHeatMap;

    // Constants for visualization and pathfinding
    static constexpr int VIEW_RADIUS = 8;
    static constexpr int MAX_PATH_LENGTH = 50;  // Limit path length
    static constexpr int PATHFINDING_TIMEOUT_MS = 50;  // Limit pathfinding time

    std::vector<Direction> findPathToFood(const Snake& snake, const sf::Vector2i& food);
    void updateHeatMapVisualization(const Snake& snake, const sf::Vector2i& food);
    bool isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const;
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    float calculateSimpleScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;

    using BaseStrategy::isPositionBlocked;
    using BaseStrategy::getManhattanDistance;
    using BaseStrategy::countAccessibleSpace;
};