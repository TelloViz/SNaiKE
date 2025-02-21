#include "AI/BasicStrategy.hpp"
#include "GameConfig.hpp"

void BasicStrategy::updateHeatMap(const Snake& snake, const sf::Vector2i& food) {
    gridHeatMap.clear();
    const sf::Vector2i& head = snake.getHead();
    Direction currentDir = snake.getCurrentDirection();
    
    // Calculate scores for visible area
    for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
            Position pos(x, y);
            float score = 0.0f;
            
            // Base distance score
            float foodDistance = getManhattanDistance(pos, Position(food));
            score = 300.0f / (1.0f + foodDistance);  // Higher base score for visibility
            
            // Immediate danger check (blocked positions)
            if (isPositionBlocked(pos, snake)) {
                score = -500.0f;  // Strong negative for blocked positions
                gridHeatMap.setValue(x, y, score);
                continue;
            }
            
            // Wall proximity penalties
            if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
                y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                score *= 0.5f;  // Reduce score near walls
            }
            
            // Favor current direction slightly (to reduce zigzagging)
            sf::Vector2i dirVec;
            switch (currentDir) {
                case Direction::Up:    dirVec = {0, -1}; break;
                case Direction::Down:  dirVec = {0, 1}; break;
                case Direction::Left:  dirVec = {-1, 0}; break;
                case Direction::Right: dirVec = {1, 0}; break;
            }
            
            float dirScore = (dirVec.x * (x - head.x) + dirVec.y * (y - head.y)) * 5.0f;
            score += dirScore;
            
            // Only set non-zero scores
            if (std::abs(score) > 0.1f) {
                gridHeatMap.setValue(x, y, score);
            }
        }
    }
    
    // Mark special positions
    gridHeatMap.setValue(food.x, food.y, 1000.0f);  // Food position
    gridHeatMap.setValue(head.x, head.y, 900.0f);   // Head position
    
    // Mark possible next moves
    sf::Vector2i nextPos = head;
    std::vector<Direction> possibleMoves = {Direction::Up, Direction::Down, 
                                          Direction::Left, Direction::Right};
    
    for (Direction dir : possibleMoves) {
        nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        if (nextPos.x >= 0 && nextPos.x < GameConfig::GRID_WIDTH &&
            nextPos.y >= 0 && nextPos.y < GameConfig::GRID_HEIGHT) {
            if (isMoveSafe(dir, snake)) {
                // Mark safe moves with medium-high value
                gridHeatMap.setValue(nextPos.x, nextPos.y, 600.0f);
            }
        }
    }
    
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