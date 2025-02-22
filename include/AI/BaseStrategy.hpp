#pragma once
#include <SFML/Graphics.hpp>
#include "Snake.hpp"
#include "Game/Direction.hpp"
#include "Game/Position.hpp"
#include <vector>
#include <iostream>

class BaseStrategy {
protected:
    static bool globalShowHeatMap;  // Add this static member
    bool isPositionBlocked(const Position& pos, const Snake& snake) const;
    bool isMoveSafe(Direction dir, const Snake& snake) const;
    
    // Manhattan distance overloads - all return float consistently
    float getManhattanDistance(const Position& a, const Position& b) const {
        return static_cast<float>(std::abs(a.pos.x - b.pos.x) + std::abs(a.pos.y - b.pos.y));
    }
    
    float getManhattanDistance(const sf::Vector2i& a, const sf::Vector2i& b) const {
        return static_cast<float>(std::abs(a.x - b.x) + std::abs(a.y - b.y));
    }
    
    float getManhattanDistance(const Position& a, const sf::Vector2i& b) const {
        return static_cast<float>(std::abs(a.pos.x - b.x) + std::abs(a.pos.y - b.y));
    }
    
    float getManhattanDistance(const sf::Vector2i& a, const Position& b) const {
        return static_cast<float>(std::abs(a.x - b.pos.x) + std::abs(a.y - b.pos.y));
    }

    int countAccessibleSpace(const Position& start, const Snake& snake) const;
    std::vector<Position> getNeighbors(const Position& pos) const;
    Direction getDirectionToNeighbor(const Position& from, const Position& to) const;
    Direction calculateRandomMove(const Snake& snake) const;

public:
    virtual ~BaseStrategy() = default;
    virtual Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) const {};
    virtual void toggleHeatMap() = 0;  // Make it pure virtual
    virtual bool isHeatMapEnabled() const { return globalShowHeatMap; }
};