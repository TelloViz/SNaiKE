#pragma once
#include "AI/BaseStrategy.hpp"
#include "GameConfig.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <map>

class AStarStrategy : public BaseStrategy {
    const Snake& snake;  // Add reference to snake for visualization

public:
    AStarStrategy(const Snake& snakeRef) : snake(snakeRef) {}
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    void render(sf::RenderWindow& window) const override;

private:
    std::vector<Direction> currentPath;
    sf::Clock pathUpdateClock;
    static constexpr float PATH_UPDATE_INTERVAL = 0.1f; // 100ms

    struct Node {
        Position pos;
        float gCost;
        float hCost;
        float fCost() const { return gCost + hCost; }
        bool operator>(const Node& other) const { return fCost() > other.fCost(); }
    };

    std::vector<Direction> findPath(const Snake& snake, const sf::Vector2i& food);
    float calculateHeuristic(const Position& pos, const sf::Vector2i& goal) const;

    // Add this to the private section
    mutable std::vector<sf::Vector2i> exploredNodes;
};