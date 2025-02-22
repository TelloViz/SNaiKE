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
    if (plannedMoves.empty()) {
        planNextMove();
    }
    
    if (!plannedMoves.empty()) {
        GameInput input = plannedMoves.front();
        plannedMoves.pop();
        return input;
    }
    
    return GameInput{InputType::ButtonPressed, GameButton::None};
}

void AIPlayer::setStrategy(AIStrategy strategy) {
    switch (strategy) {
        case AIStrategy::Manhattan:
            currentStrategy = std::make_unique<ManhattanStrategy>();
            break;
        case AIStrategy::AStar:
            currentStrategy = std::make_unique<AStarStrategy>(snake);  // Pass snake reference
            break;
        case AIStrategy::None:
            currentStrategy.reset();
            break;
    }
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

