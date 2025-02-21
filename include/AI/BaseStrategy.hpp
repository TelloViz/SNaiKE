#pragma once
#include "ISnakeStrategy.hpp"
#include <queue>
#include <vector>
#include <SFML/System/Clock.hpp>

class BaseStrategy : public ISnakeStrategy {
public:
    virtual ~BaseStrategy() = default;
    virtual const HeatMap& getHeatMap() const override { return heatMap; }

protected:
    bool isMoveSafe(Direction dir, const Snake& snake) const;
    bool isPositionBlocked(const Position& pos, const Snake& snake) const;
    int getManhattanDistance(const Position& a, const Position& b) const;
    int countAccessibleSpace(const Position& from, const Snake& snake) const;
    std::vector<Position> getNeighbors(const Position& pos) const;
    Direction getDirectionToNeighbor(const Position& from, const Position& to) const;
    Direction calculateRandomMove(const Snake& snake) const;
};