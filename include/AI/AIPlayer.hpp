#pragma once
#include "Input/InputHandler.hpp"
#include "Snake.hpp"
#include "AI/HeatMap.hpp"
#include "AI/GridHeatMap.hpp"
#include <queue>
#include <SFML/System/Clock.hpp>
#include <algorithm>
#include <map>
#include <set>

enum class AIStrategy {
    Basic = 0,
    Advanced = 1,
    Random = 2,
    COUNT
};

struct Position {
    sf::Vector2i pos{0, 0};  // Initialize with default values
    
    // Add default constructor
    Position() : pos(0, 0) {}
    Position(const sf::Vector2i& p) : pos(p) {}
    Position(int x, int y) : pos(x, y) {}
    
    bool operator<(const Position& other) const {
        return pos.x < other.pos.x || (pos.x == other.pos.x && pos.y < other.pos.y);
    }
    
    bool operator==(const Position& other) const {
        return pos.x == other.pos.x && pos.y == other.pos.y;
    }
    
    operator sf::Vector2i() const { return pos; }
};

struct Node {
    Position pos;  // Change this from Vector2i to Position
    int g{0};  // Cost from start
    int h{0};  // Heuristic to goal
    int f{0};  // f = g + h
    Direction dirFromParent;
    
    Node(const Position& p, int g_, int h_) 
        : pos(p), g(g_), h(h_), f(g_ + h_) {}
    
    bool operator<(const Node& other) const {
        return f < other.f || (f == other.f && h < other.h);
    }
};

class AIPlayer {
private:
    std::queue<GameInput> plannedMoves;
    const Snake& snake;
    const sf::Vector2i& food;
    AIStrategy currentStrategy;
    HeatMap basicHeatMap;
    HeatMap advancedHeatMap;
    HeatMap heatMap;  // Add this line
    GridHeatMap gridHeatMap;
    sf::Vector2i lastSnakePosition;
    std::vector<Direction> lastPath;

    void planNextMove();
    Direction calculateDirectionToFood();
    Direction calculateBasicMove();
    Direction calculateAdvancedMove();
    Direction calculateRandomMove();
    
    void updateBasicHeatMap();
    void updateAdvancedHeatMap();
    void updateHeatMap();  // Add the updateHeatMap declaration
    
    bool isMoveSafe(Direction dir);
    bool isMoveSafe(Direction dir, int lookAhead);
    bool isMoveSafeInFuture(Direction dir, int lookAhead = 5);
    bool willTailMove(const sf::Vector2i& pos, int segmentIndex);
    int countAccessibleSpace(const Position& from) const;
    bool canReachFood(const Position& from);
    std::vector<Direction> findPathToFood();
    int getManhattanDistance(const Position& a, const Position& b) const;
    std::vector<Position> getNeighbors(const Position& pos);
    Direction getDirectionToNeighbor(const Position& from, const Position& to);
    bool isPositionBlocked(const Position& pos);
    int getDistanceToTail(const sf::Vector2i& pos);
    float calculatePositionScore(int x, int y) const;

public:
    AIPlayer(const Snake& snakeRef, const sf::Vector2i& foodRef) 
        : snake(snakeRef)
        , food(foodRef)
        , currentStrategy(AIStrategy::Basic) {}
    
    GameInput getNextInput();
    void setStrategy(AIStrategy strategy) { currentStrategy = strategy; }
    AIStrategy getStrategy() const { return currentStrategy; }
    const HeatMap& getHeatMap() const {
        return (currentStrategy == AIStrategy::Advanced) ? advancedHeatMap : basicHeatMap;
    }
    const GridHeatMap& getGridHeatMap() const { return gridHeatMap; }
};