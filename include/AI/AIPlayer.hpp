#pragma once
#include <SFML/Graphics.hpp>
#include <queue>
#include "Input/InputHandler.hpp"
#include "Snake.hpp"
#include "AI/BaseStrategy.hpp"
#include "AI/ManhattanStrategy.hpp"
#include "AI/AStarStrategy.hpp"
#include "AI/AdvancedStrategy.hpp"
#include <memory>

enum class AIStrategy {
    None,
    Random,
    Manhattan,
    AStar  // Only keep what we're actually using
};

class AIPlayer {
public:
    AIPlayer(const Snake& s, const sf::Vector2i& f) 
        : snake(s), food(f), currentStrategyType(AIStrategy::Manhattan) {
        setStrategy(currentStrategyType);
    }

    GameInput getNextInput();
    void setStrategy(AIStrategy type);
    void planNextMove();

    AIStrategy getStrategy() const { 
        return currentStrategyType;
    }

    bool isEnabled() const { return currentStrategy != nullptr; }
    BaseStrategy* getCurrentStrategy() const { return currentStrategy.get(); }

private:
    GameButton directionToButton(Direction dir);
    std::unique_ptr<BaseStrategy> currentStrategy;
    std::queue<GameInput> plannedMoves;
    const Snake& snake;
    const sf::Vector2i& food;
    AIStrategy currentStrategyType;
};