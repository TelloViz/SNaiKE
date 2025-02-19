#include "AI/AIPlayer.hpp"
#include "GameConfig.hpp"
#include <iostream>

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

void AIPlayer::planNextMove() {
    Direction targetDir = calculateDirectionToFood();
    
    // Convert direction to GameInput
    GameButton button = GameButton::None;
    switch (targetDir) {
        case Direction::Up:    button = GameButton::Up; break;
        case Direction::Down:  button = GameButton::Down; break;
        case Direction::Left:  button = GameButton::Left; break;
        case Direction::Right: button = GameButton::Right; break;
    }
    
    // Always push the move - isMoveSafe was already checked in calculateDirectionToFood
    if (button != GameButton::None) {
        std::cout << "AI planning move: " << static_cast<int>(button) << std::endl;  // Debug output
        plannedMoves.push(GameInput{InputType::ButtonPressed, button});
    }
}

Direction AIPlayer::calculateDirectionToFood() {
    switch (currentStrategy) {
        case AIStrategy::Basic:
            return calculateBasicMove();  // Current logic
        case AIStrategy::Advanced:
            return calculateAdvancedMove();
        case AIStrategy::Random:
            return calculateRandomMove();
        default:
            return calculateBasicMove();
    }
}

Direction AIPlayer::calculateAdvancedMove() {
    const sf::Vector2i& head = snake.getHead();
    std::vector<std::pair<Direction, int>> possibleMoves;
    
    // Look ahead multiple moves
    const int LOOKAHEAD = 5;
    
    auto checkMove = [&](Direction dir) {
        if (!isMoveSafe(dir, LOOKAHEAD)) return;
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        int score = 0;
        
        // Check if we can reach food from this position
        if (canReachFood(nextPos)) {
            score -= 1000;  // Heavy bonus for moves that keep food reachable
        }
        
        // Prefer moves that maximize available space
        int spaceScore = countAccessibleSpace(nextPos);
        score -= spaceScore;
        
        possibleMoves.push_back({dir, score});
    };
    
    // Check all directions
    checkMove(Direction::Up);
    checkMove(Direction::Down);
    checkMove(Direction::Left);
    checkMove(Direction::Right);
    
    if (!possibleMoves.empty()) {
        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        return possibleMoves[0].first;
    }
    
    return snake.getCurrentDirection();
}

Direction AIPlayer::calculateRandomMove() {
    std::vector<Direction> safeMoves;
    
    // Collect all safe moves
    if (isMoveSafe(Direction::Up)) safeMoves.push_back(Direction::Up);
    if (isMoveSafe(Direction::Down)) safeMoves.push_back(Direction::Down);
    if (isMoveSafe(Direction::Left)) safeMoves.push_back(Direction::Left);
    if (isMoveSafe(Direction::Right)) safeMoves.push_back(Direction::Right);
    
    if (!safeMoves.empty()) {
        // Pick a random safe move
        return safeMoves[rand() % safeMoves.size()];
    }
    
    return snake.getCurrentDirection();
}

Direction AIPlayer::calculateBasicMove() {
    const sf::Vector2i& head = snake.getHead();
    std::vector<std::pair<Direction, int>> possibleMoves;
    
    int currentDistance = std::abs(food.x - head.x) + std::abs(food.y - head.y);
    
    // Check each direction and score it
    auto checkMove = [&](Direction dir) {
        if (!isMoveSafe(dir)) return;
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        int newDistance = std::abs(food.x - nextPos.x) + std::abs(food.y - nextPos.y);
        int score = newDistance * 10;
        
        if (newDistance < currentDistance) {
            score -= 5;
        }
        
        if (nextPos.x == 0 || nextPos.x == GameConfig::GRID_WIDTH - 1 ||
            nextPos.y == 0 || nextPos.y == GameConfig::GRID_HEIGHT - 1) {
            score += 3;
        }
        
        possibleMoves.push_back({dir, score});
    };
    
    checkMove(Direction::Up);
    checkMove(Direction::Down);
    checkMove(Direction::Left);
    checkMove(Direction::Right);
    
    if (!possibleMoves.empty()) {
        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        return possibleMoves[0].first;
    }
    
    return snake.getCurrentDirection();
}

bool AIPlayer::isMoveSafe(Direction dir) {
    sf::Vector2i nextPos = snake.getHead();
    switch (dir) {
        case Direction::Up:    nextPos.y--; break;
        case Direction::Down:  nextPos.y++; break;
        case Direction::Left:  nextPos.x--; break;
        case Direction::Right: nextPos.x++; break;
    }
    
    // Check wall collision
    if (nextPos.x < 0 || nextPos.x >= GameConfig::GRID_WIDTH ||
        nextPos.y < 0 || nextPos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }
    
    // Check self collision
    const std::deque<sf::Vector2i>& snakeBody = snake.getBody();
    for (size_t i = 0; i < snakeBody.size(); ++i) {
        if (nextPos.x == snakeBody[i].x && nextPos.y == snakeBody[i].y) {
            return false;
        }
    }
    
    return true;
}

bool AIPlayer::isMoveSafe(Direction dir, int lookAhead) {
    if (lookAhead <= 0) return true;
    
    sf::Vector2i nextPos = snake.getHead();
    switch (dir) {
        case Direction::Up:    nextPos.y--; break;
        case Direction::Down:  nextPos.y++; break;
        case Direction::Left:  nextPos.x--; break;
        case Direction::Right: nextPos.x++; break;
    }
    
    // Basic collision checks
    if (nextPos.x < 0 || nextPos.x >= GameConfig::GRID_WIDTH ||
        nextPos.y < 0 || nextPos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }
    
    const auto& body = snake.getBody();
    for (const auto& segment : body) {
        if (nextPos == segment) return false;
    }
    
    // Look ahead recursively if needed
    if (lookAhead > 1) {
        bool foundSafeMove = false;
        // Check if at least one next move would be safe
        if (isMoveSafe(Direction::Up, lookAhead - 1)) foundSafeMove = true;
        if (isMoveSafe(Direction::Down, lookAhead - 1)) foundSafeMove = true;
        if (isMoveSafe(Direction::Left, lookAhead - 1)) foundSafeMove = true;
        if (isMoveSafe(Direction::Right, lookAhead - 1)) foundSafeMove = true;
        return foundSafeMove;
    }
    
    return true;
}

// Add implementation for countAccessibleSpace
int AIPlayer::countAccessibleSpace(const sf::Vector2i& from) {
    std::vector<std::vector<bool>> visited(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
    
    int space = 0;
    std::queue<sf::Vector2i> toVisit;
    toVisit.push(from);
    
    while (!toVisit.empty()) {
        sf::Vector2i pos = toVisit.front();
        toVisit.pop();
        
        if (pos.x < 0 || pos.x >= GameConfig::GRID_WIDTH || 
            pos.y < 0 || pos.y >= GameConfig::GRID_HEIGHT ||
            visited[pos.x][pos.y]) {
            continue;
        }
        
        visited[pos.x][pos.y] = true;
        space++;
        
        // Check if this position contains snake body
        bool isSnakeBody = false;
        const std::deque<sf::Vector2i>& snakeBody = snake.getBody();
        for (size_t i = 0; i < snakeBody.size(); ++i) {
            if (pos.x == snakeBody[i].x && pos.y == snakeBody[i].y) {
                isSnakeBody = true;
                break;
            }
        }
        if (isSnakeBody) continue;
        
        // Add adjacent cells
        toVisit.push(sf::Vector2i(pos.x + 1, pos.y));
        toVisit.push(sf::Vector2i(pos.x - 1, pos.y));
        toVisit.push(sf::Vector2i(pos.x, pos.y + 1));
        toVisit.push(sf::Vector2i(pos.x, pos.y - 1));
    }
    
    return space;
}

// Add implementation for canReachFood
bool AIPlayer::canReachFood(const sf::Vector2i& from) {
    return countAccessibleSpace(from) > 0;  // Simplified version
}