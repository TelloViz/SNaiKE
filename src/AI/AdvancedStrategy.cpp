#include "AI/AdvancedStrategy.hpp"
#include "GameConfig.hpp"
#include <queue>
#include <map>
#include <set>
#include <iostream>

Direction AdvancedStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    std::cout << "Advanced Strategy: Starting move calculation" << std::endl;
    
    // Handle stuck detection
    if (snake.getHead() == lastPosition) {
        std::cout << "Stuck detection triggered" << std::endl;
        stuckCount++;
        if (stuckCount > 3) {
            stuckCount = 0;
            return calculateRandomMove(snake);
        }
    } else {
        stuckCount = 0;
        lastPosition = snake.getHead();
    }
    
    // Try to find path to food
    auto path = findPathToFood(snake, food);
    if (!path.empty()) {
        Position nextPos(snake.getHead());
        Direction firstMove = path[0];
        
        switch (firstMove) {
            case Direction::Up:    nextPos.pos.y--; break;
            case Direction::Down:  nextPos.pos.y++; break;
            case Direction::Left:  nextPos.pos.x--; break;
            case Direction::Right: nextPos.pos.x++; break;
        }
        
        if (countAccessibleSpace(nextPos, snake) >= snake.getBody().size()) {
            return firstMove;
        }
    }

    // Fallback to scored moves
    std::vector<std::pair<Direction, float>> possibleMoves;
    const sf::Vector2i& head = snake.getHead();
    
    for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        if (!isMoveSafeInFuture(dir, 5, snake)) continue;
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        float moveScore = calculatePositionScore(nextPos.x, nextPos.y, snake, food);
        possibleMoves.push_back({dir, moveScore});
    }
    
    if (!possibleMoves.empty()) {
        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        return possibleMoves[0].first;
    }
    
    return snake.getCurrentDirection();
}

void AdvancedStrategy::update() {
    // No update needed for now
}

std::vector<Direction> AdvancedStrategy::findPathToFood(const Snake& snake, const sf::Vector2i& food) {
    std::cout << "Finding path to food" << std::endl;
    std::cout << "Snake head: " << snake.getHead().x << "," << snake.getHead().y << std::endl;
    std::cout << "Food: " << food.x << "," << food.y << std::endl;
    
    Position start(snake.getHead());
    Position goal(food);
    
    // Add a maximum number of iterations to prevent infinite loops
    int maxIterations = GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT;
    int iterations = 0;
    
    std::set<Position> visited;
    std::queue<std::pair<Position, std::vector<Direction>>> queue;
    queue.push({start, std::vector<Direction>()});
    visited.insert(start);
    
    while (!queue.empty() && iterations < maxIterations) {
        iterations++;
        auto [current, path] = queue.front();
        queue.pop();
        
        if (current.pos == food) {
            std::cout << "Path found to food" << std::endl;
            return path;
        }
        
        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = current;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
                default: continue;
            }
            
            if (visited.count(next) || !isMoveSafe(dir, snake)) {
                continue;
            }
            
            auto nextPath = path;
            nextPath.push_back(dir);
            queue.push({next, nextPath});
            visited.insert(next);
        }
    }
    
    std::cout << "Path finding completed after " << iterations << " iterations" << std::endl;
    return std::vector<Direction>();
}

float AdvancedStrategy::calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const {
    Position pos(x, y);
    float score = 0.0f;
    
    // Distance to food
    score -= getManhattanDistance(pos, Position(food)) * 5.0f;
    
    // Wall proximity penalty
    int wallDist = std::min({x, y, GameConfig::GRID_WIDTH - 1 - x, 
                           GameConfig::GRID_HEIGHT - 1 - y});
    score += wallDist * 2.0f;
    
    // Available space bonus
    score += countAccessibleSpace(pos, snake) * 0.5f;
    
    // Penalty for being in corners
    if ((x == 0 || x == GameConfig::GRID_WIDTH - 1) &&
        (y == 0 || y == GameConfig::GRID_HEIGHT - 1)) {
        score -= 50.0f;
    }
    
    return score;
}

bool AdvancedStrategy::isMoveSafeInFuture(Direction dir, int lookAhead, const Snake& snake) const {
    if (lookAhead <= 0) return true;
    
    sf::Vector2i nextPos = snake.getHead();
    switch (dir) {
        case Direction::Up:    nextPos.y--; break;
        case Direction::Down:  nextPos.y++; break;
        case Direction::Left:  nextPos.x--; break;
        case Direction::Right: nextPos.x++; break;
    }
    
    Position pos(nextPos);
    
    // Basic safety checks
    if (isPositionBlocked(pos, snake)) {
        return false;
    }
    
    // Check available space
    int space = countAccessibleSpace(pos, snake);
    if (space < snake.getBody().size()) {
        return false;
    }
    
    // Look ahead recursively if needed
    if (lookAhead > 1) {
        for (Direction nextDir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            if (isMoveSafeInFuture(nextDir, lookAhead - 1, snake)) {
                return true;
            }
        }
        return false;
    }
    
    return true;
}