#pragma once
#include "Input/InputHandler.hpp"
#include "Snake.hpp"
#include "AI/HeatMap.hpp"
#include "AI/GridHeatMap.hpp"
#include "AI/ISnakeStrategy.hpp"
#include <memory>
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

// struct Position {
//     sf::Vector2i pos{0, 0};  // Initialize with default values
    
//     // Add default constructor
//     Position() : pos(0, 0) {}
//     Position(const sf::Vector2i& p) : pos(p) {}
//     Position(int x, int y) : pos(x, y) {}
    
//     bool operator<(const Position& other) const {
//         return pos.x < other.pos.x || (pos.x == other.pos.x && pos.y < other.pos.y);
//     }
    
//     bool operator==(const Position& other) const {
//         return pos.x == other.pos.x && pos.y == other.pos.y;
//     }
    
//     bool operator!=(const Position& other) const {
//         return pos != other.pos;
//     }
    
//     operator sf::Vector2i() const { return pos; }
// };

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
    std::unique_ptr<ISnakeStrategy> strategy;
    std::queue<GameInput> plannedMoves;
    const Snake& snake;
    const sf::Vector2i& food;
    HeatMap basicHeatMap;
    HeatMap advancedHeatMap;
    GridHeatMap gridHeatMap;
    AIStrategy currentStrategy;  // Add this line to track current strategy

    void planNextMove();
    GameButton directionToButton(Direction dir);

public:
    AIPlayer(const Snake& snakeRef, const sf::Vector2i& foodRef) 
        : snake(snakeRef)
        , food(foodRef)
        , currentStrategy(AIStrategy::Advanced) {
        setStrategy(AIStrategy::Advanced);  // Initialize with Advanced strategy
    }
    
    GameInput getNextInput();
    void setStrategy(AIStrategy type);
    AIStrategy getStrategy() const { return currentStrategy; }  // Add this getter
    
    // Heat map getters for visualization
    const HeatMap& getHeatMap() const { 
        return advancedHeatMap; 
    }
    const GridHeatMap& getGridHeatMap() const { 
        return gridHeatMap; 
    }
};