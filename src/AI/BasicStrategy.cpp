#include "AI/BasicStrategy.hpp"
#include "GameConfig.hpp"

Direction BasicStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    const sf::Vector2i& head = snake.getHead();
    std::vector<std::pair<Direction, int>> possibleMoves;
    
    int currentDistance = std::abs(food.x - head.x) + std::abs(food.y - head.y);
    
    // Check each direction and score it
    auto checkMove = [&](Direction dir) {
        if (!isMoveSafe(dir, snake)) return;
        
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

float BasicStrategy::calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const {
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
    
    return score;
}

void BasicStrategy::update() {
    // Basic strategy doesn't need regular updates
}