#pragma once
#include "AI/BaseStrategy.hpp"
#include <SFML/System/Vector2.hpp>

class AdvancedStrategy : public BaseStrategy {
private:
    sf::Vector2i lastPosition;
    int stuckCount{0};
    std::vector<Direction> lastPath;

public:
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;

private:
    sf::Clock updateClock;
    sf::Clock stuckClock;

    std::vector<Direction> findPathToFood(const Snake& snake, const sf::Vector2i& food);
    float calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const;
    bool isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const;
    int getDistanceToTail(const sf::Vector2i& pos, const Snake& snake) const;
    bool willTailMove(const sf::Vector2i& pos, int segmentIndex, const Snake& snake) const;
    int calculateHeuristic(const Position& pos, const Snake& snake, const sf::Vector2i& food) const;
};