#pragma once
#include "BaseStrategy.hpp"
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class AdvancedStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;

private:
    sf::Vector2i lastPosition;
    std::vector<Direction> lastPath;
    sf::Clock pathfindClock;
    Position lastFoodPos;
    int stuckCount{0};
    std::vector<std::vector<float>> scoreCache;
    sf::Clock gameTimer;  // Add this for the morphing effect

    std::vector<Direction> findPathToFood(const Snake& snake, const sf::Vector2i& food);
    void updateHeatMapVisualization(const Snake& snake, const sf::Vector2i& food);
    void updateHeatMapForPath(const Snake& snake, const sf::Vector2i& food, 
                            const std::vector<Direction>& path);
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    bool isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const;

    using BaseStrategy::isPositionBlocked;  // Make base class method visible
    using BaseStrategy::getManhattanDistance;  // Make base class method visible
    using BaseStrategy::countAccessibleSpace;  // Make base class method visible
    using BaseStrategy::heatMap;  // Make base class member visible
};