#pragma once
#include "BaseStrategy.hpp"
#include <SFML/System/Vector2.hpp>

class AdvancedStrategy : public BaseStrategy {
public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;

private:
    sf::Vector2i lastPosition;
    int stuckCount{0};
    std::vector<Direction> lastPath;
    
    std::vector<Direction> findPathToFood(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    bool isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const;
};