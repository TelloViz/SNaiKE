#pragma once
#include "Input/InputHandler.hpp"
#include "Snake.hpp"
#include <queue>
#include <vector>

enum class AIStrategy {
    Basic = 0,      // Current simple strategy
    Advanced = 1,   // Nearly unbeatable strategy
    Random = 2,     // Random but safe moves (for fun)
    COUNT
};

class AIPlayer {
private:
    std::queue<GameInput> plannedMoves;
    const Snake& snake;
    const sf::Vector2i& food;
    AIStrategy currentStrategy;
    
    void planNextMove();
    Direction calculateDirectionToFood();
    Direction calculateBasicMove();
    Direction calculateAdvancedMove();
    Direction calculateRandomMove();
    bool isMoveSafe(Direction dir);  // Single-step version
    bool isMoveSafe(Direction dir, int lookAhead);  // Multi-step version
    bool canReachFood(const sf::Vector2i& from);
    int countAccessibleSpace(const sf::Vector2i& from);
    
public:
    AIPlayer(const Snake& snakeRef, const sf::Vector2i& foodRef) 
        : snake(snakeRef)
        , food(foodRef)
        , currentStrategy(AIStrategy::Basic) {}
    
    GameInput getNextInput();
    void setStrategy(AIStrategy strategy) { currentStrategy = strategy; }
    AIStrategy getStrategy() const { return currentStrategy; }
};