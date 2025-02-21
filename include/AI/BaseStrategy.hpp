#pragma once
#include <SFML/Graphics.hpp>
#include "Snake.hpp"
#include "Game/Direction.hpp"
#include "Game/Position.hpp"
#include <vector>

class BaseStrategy {
public:
    virtual ~BaseStrategy() = default;
    virtual Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) const {};

protected:
    bool isPositionBlocked(const Position& pos, const Snake& snake) const;
    bool isMoveSafe(Direction dir, const Snake& snake) const;
    int getManhattanDistance(const Position& a, const Position& b) const;
    int countAccessibleSpace(const Position& start, const Snake& snake) const;
    std::vector<Position> getNeighbors(const Position& pos) const;
    Direction getDirectionToNeighbor(const Position& from, const Position& to) const;
    Direction calculateRandomMove(const Snake& snake) const;
};