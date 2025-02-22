#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include "Input/InputHandler.hpp"
#include "Snake.hpp"
#include "AI/BaseStrategy.hpp"
#include "AI/BasicStrategy.hpp"
#include "AI/AdvancedStrategy.hpp"
#include "AI/RandomStrategy.hpp"
#include <memory>

enum class AIStrategy {
    Basic,
    Advanced,
    Random
};

class AIPlayer {
public:
    AIPlayer(const Snake& snakeRef, const sf::Vector2i& foodRef) 
        : snake(const_cast<Snake&>(snakeRef))  // Need const_cast since we modify snake
        , food(foodRef) {}

    GameInput getNextInput();
    void setStrategy(AIStrategy strategyType);
    void planNextMove();

    AIStrategy getStrategy() const { 
        if (dynamic_cast<BasicStrategy*>(currentStrategy.get())) return AIStrategy::Basic;
        if (dynamic_cast<AdvancedStrategy*>(currentStrategy.get())) return AIStrategy::Advanced;
        if (dynamic_cast<RandomStrategy*>(currentStrategy.get())) return AIStrategy::Random;
        return AIStrategy::Basic; // default
    }

    // If you need heat map access
    const BaseStrategy* getStrategyPtr() const { return currentStrategy.get(); }

    // Add these new methods
    bool isEnabled() const { return currentStrategy != nullptr; }
    BaseStrategy* getCurrentStrategy() const { return currentStrategy.get(); }

private:
    GameButton directionToButton(Direction dir);
    std::unique_ptr<BaseStrategy> currentStrategy;  // Forward declaration with unique_ptr
    std::queue<GameInput> plannedMoves;
    Snake& snake;
    const sf::Vector2i& food;
};