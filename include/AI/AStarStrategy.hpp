#pragma once
#include "AI/BaseStrategy.hpp"
#include "Snake.hpp"
#include "GameConfig.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <map>
#include <cmath>

class AStarStrategy : public BaseStrategy {
    const Snake& snake;  // Add reference to snake for visualization

public:
    enum class Heuristic {
        MANHATTAN,    // Key 2
        EUCLIDEAN,   // Key 3
        CHEBYSHEV    // Key 4
    };

    AStarStrategy(const Snake& snakeRef);// : snake(snakeRef), currentHeuristic(Heuristic::MANHATTAN) {}
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    void render(sf::RenderWindow& window) const override;
    void setHeuristic(Heuristic h) { currentHeuristic = h; }
    Heuristic getHeuristic() const { return currentHeuristic; }
    void toggleHeatMap();// { showHeatMap = !showHeatMap; }
    void togglePathArrows();
    bool isHeatMapEnabled() const { return showHeatMap; }
    bool isPathArrowsEnabled() const { return showPathArrows; }
    void setVisualizationState(bool heatMap, bool pathArrows) {
        showHeatMap = heatMap;
        showPathArrows = pathArrows;
    }

private:
    std::vector<Direction> currentPath;
    std::vector<Direction> visualPath;  // Add this for visualization
    sf::Clock pathUpdateClock;


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
    Heuristic currentHeuristic;

    bool isPositionSafe(const Position& pos, const Snake& snake) const;
    int countAccessibleSpace(const Position& start, const Snake& snake) const;
    std::vector<Position> getNeighbors(const Position& pos) const;

    // Add these method declarations
    bool isMovingTowardsBody(const Position& pos, Direction dir, const Snake& snake) const;
    bool isPathSafe(const std::vector<Direction>& path, const Snake& snake) const;
    std::vector<Direction> reconstructPath(
        const Position& start,
        const Position& goal,
        const std::map<Position, Position>& cameFrom,
        const std::map<Position, Direction>& directionToParent) const;

    // Add this helper method declaration
    float getEuclideanDistanceSquared(const sf::Vector2i& a, const sf::Vector2i& b) const;

    int countAccessibleNeighbors(const Position& pos) const;

    mutable sf::Clock renderClock;
    mutable sf::Clock explorationRenderClock;
    static constexpr float PATH_RENDER_INTERVAL = 0.1f;      // Slower arrow updates
    static constexpr float PATH_UPDATE_INTERVAL = 0.1f; // 100ms
    static constexpr float EXPLORATION_RENDER_INTERVAL = 0.1f; // Faster heat map updates
    mutable std::vector<sf::Vector2i> lastExploredNodes;     // Store last exploration state
    mutable bool hasExplorationData = false;                 // Track if we have data to show
    mutable bool showHeatMap = false;
    mutable bool showPathArrows = false;
};