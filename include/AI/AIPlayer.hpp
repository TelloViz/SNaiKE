#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include "Input/InputHandler.hpp"
#include "Snake.hpp"
#include "AI/BaseStrategy.hpp"
#include "AI/ManhattanStrategy.hpp"
#include "AI/AdvancedStrategy.hpp"
#include <memory>

enum class AIStrategy {
    None,
    Manhattan,
    Advanced,
    Random,
    AStar  // renamed from Advanced to be more explicit
};

class AIPlayer {
public:
    AIPlayer(const Snake& snakeRef, const sf::Vector2i& foodRef) 
        : snake(const_cast<Snake&>(snakeRef))
        , food(foodRef) {}

    GameInput getNextInput();
    void setStrategy(AIStrategy strategyType);
    void planNextMove();

    AIStrategy getStrategy() const { 
        if (dynamic_cast<ManhattanStrategy*>(currentStrategy.get())) return AIStrategy::Manhattan;
        if (dynamic_cast<AdvancedStrategy*>(currentStrategy.get())) return AIStrategy::AStar;
        return AIStrategy::None;
    }

    bool isEnabled() const { return currentStrategy != nullptr; }
    BaseStrategy* getCurrentStrategy() const { return currentStrategy.get(); }

private:
    GameButton directionToButton(Direction dir);
    std::unique_ptr<BaseStrategy> currentStrategy;
    std::queue<GameInput> plannedMoves;
    Snake& snake;
    const sf::Vector2i& food;
};