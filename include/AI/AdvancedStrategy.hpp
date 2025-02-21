#pragma once
#include "BaseStrategy.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>

class AdvancedStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;

private:
    sf::Vector2i lastPosition;
    std::vector<Direction> lastPath;
    int stuckCount{0};
    std::vector<std::vector<float>> scoreCache;

    std::vector<Direction> findPathToFood(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    bool isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const;
    void updateHeatMapForPath(const Snake& snake, const sf::Vector2i& food, 
                            const std::vector<Direction>& path);
    void updateHeatMap(const Snake& snake, const sf::Vector2i& food);
};