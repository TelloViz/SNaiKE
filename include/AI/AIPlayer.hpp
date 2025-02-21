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
    AIStrategy currentStrategy;

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
    AIStrategy getStrategy() const { return currentStrategy; }
    const HeatMap& getHeatMap() const { return strategy->getHeatMap(); }
};