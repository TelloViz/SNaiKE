#pragma once
#include "AI/AIPlayer.hpp"
#include "GameConfig.hpp"
#include <iostream>
#include "Debug.hpp"
#include <algorithm>
#include <limits>
#include <queue>
#include <set>
#include <map>
#include "AI/BasicStrategy.hpp"
#include "AI/AdvancedStrategy.hpp"
#include "AI/RandomStrategy.hpp"

using std::min;
using std::max;

GameInput AIPlayer::getNextInput() {
    if (!currentStrategy) return GameInput{};
    
    Direction nextMove = currentStrategy->calculateNextMove(snake, food);
    GameButton button;
    
    switch (nextMove) {
        case Direction::Up:    button = GameButton::Up; break;
        case Direction::Down:  button = GameButton::Down; break;
        case Direction::Left:  button = GameButton::Left; break;
        case Direction::Right: button = GameButton::Right; break;
        default: button = GameButton::None; break;
    }
    
    return GameInput{InputType::ButtonPressed, button};
}

void AIPlayer::setStrategy(AIStrategy type) {
    bool heatMapEnabled = false;
    bool pathArrowsEnabled = false;
    
    // Store current visualization states if existing strategy is A*
    if (auto* currentAstar = dynamic_cast<AStarStrategy*>(currentStrategy.get())) {
        heatMapEnabled = currentAstar->isHeatMapEnabled();
        pathArrowsEnabled = currentAstar->isPathArrowsEnabled();
    }

    // Create new strategy
    switch (type) {
        case AIStrategy::Manhattan:
            currentStrategy = std::make_unique<ManhattanStrategy>();
            break;
        case AIStrategy::AStar:
            {
                auto newStrategy = std::make_unique<AStarStrategy>(snake);
                if (auto* astar = dynamic_cast<AStarStrategy*>(newStrategy.get())) {
                    astar->setVisualizationState(heatMapEnabled, pathArrowsEnabled);
                }
                currentStrategy = std::move(newStrategy);
            }
            break;
    }
    
    currentStrategyType = type;
}

void AIPlayer::planNextMove() {
    std::cout << "Planning next move with strategy type: " 
              << (currentStrategy ? "Active" : "None") << std::endl;
    Direction targetDir = currentStrategy->calculateNextMove(snake, food);
    std::cout << "Got direction: " << static_cast<int>(targetDir) << std::endl;
    GameButton button = directionToButton(targetDir);
    if (button != GameButton::None) {
        plannedMoves.push(GameInput{InputType::ButtonPressed, button});
    }
}

GameButton AIPlayer::directionToButton(Direction dir) {
    switch (dir) {
        case Direction::Up:    return GameButton::Up;
        case Direction::Down:  return GameButton::Down;
        case Direction::Left:  return GameButton::Left;
        case Direction::Right: return GameButton::Right;
        default:              return GameButton::None;
    }
}

// Get heatmap defined in header

