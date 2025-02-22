#include "AI/BasicStrategy.hpp"
#include "GameConfig.hpp"

void BasicStrategy::updateHeatMap(const Snake& snake, const sf::Vector2i& food) {
    gridHeatMap.clear();
    const sf::Vector2i& head = snake.getHead();
    Direction currentDir = snake.getCurrentDirection();

    // First pass: Basic distance scoring
    for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
            sf::Vector2i vector(x, y);
            Position pos{vector};  // Use uniform initialization
            float score = 0.0f;
            
            // Basic move scoring
            if (!isPositionBlocked(Position(pos), snake)) {
                // Score based on distance to food
                float foodDistance = BaseStrategy::getManhattanDistance(pos, food);
                score = 100.0f / (1.0f + foodDistance);
                
                // Wall penalty
                if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
                    y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                    score *= 0.5f;
                }
            }

            if (std::abs(score) > 0.1f) {
                gridHeatMap.setValue(x, y, score);
            }
        }
    }

    // Mark possible moves
    std::vector<Direction> moves = {Direction::Up, Direction::Down, 
                                  Direction::Left, Direction::Right};
    for (Direction dir : moves) {
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        if (isValidPosition(nextPos) && isMoveSafe(dir, snake)) {
            float currentDist = BaseStrategy::getManhattanDistance(head, food);
            float newDist = BaseStrategy::getManhattanDistance(nextPos, food);
            
            if (newDist < currentDist) {
                gridHeatMap.setValue(nextPos.x, nextPos.y, 700.0f);  // Best moves
            } else {
                gridHeatMap.setValue(nextPos.x, nextPos.y, 600.0f);  // Safe moves
            }
        }
    }

    // Mark critical positions
    gridHeatMap.setValue(food.x, food.y, 1000.0f);  // Food position
    gridHeatMap.setValue(head.x, head.y, 900.0f);   // Head position

    gridHeatMap.triggerUpdate();
}

Direction BasicStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    // Update heat map first
    updateHeatMap(snake, food);
    
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

float BasicStrategy::calculatePositionScore(const sf::Vector2i& pos, const Snake& snake, const sf::Vector2i& food) const {
    if (isPositionBlocked(Position(pos), snake)) {
        return -1000.0f;
    }

    float score = 0.0f;
    
    // Distance to food
    float foodDistance = BaseStrategy::getManhattanDistance(pos, food);
    score += 100.0f / (1.0f + foodDistance);
    
    // Wall penalty
    if (pos.x == 0 || pos.x == GameConfig::GRID_WIDTH - 1 ||
        pos.y == 0 || pos.y == GameConfig::GRID_HEIGHT - 1) {
        score *= 0.5f;
    }
    
    return score;
}

void BasicStrategy::update() {
    // Basic strategy doesn't need regular updates
}