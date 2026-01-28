#pragma once

// Standard Library
#include <memory>
#include <queue>

// Third-Party
#include <SFML/Graphics.hpp>

// Project
#include "input/InputHandler.hpp"
#include "Snake.hpp"
#include "ai/BaseStrategy.hpp"
#include "ai/ManhattanStrategy.hpp"
#include "ai/AStarStrategy.hpp"
#include "ai/AdvancedStrategy.hpp"


enum class AIStrategy {
    Manhattan,
    AStar,
    Hamilton,
    FloodFill,
    Voronoi,
    HybridAStar,
    WallFollow,
    None
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
    AIStrategy currentAIStrategy = AIStrategy::None;
};