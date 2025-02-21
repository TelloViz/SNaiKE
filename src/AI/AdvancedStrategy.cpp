#include "AI/AdvancedStrategy.hpp"
#include "GameConfig.hpp"
#include <queue>
#include <map>
#include <set>
#include <iostream>

Direction AdvancedStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    // Try to find path to food first using A*
    lastPath = findPathToFood(snake, food);
    if (!lastPath.empty()) {
        Direction nextDir = lastPath.front();
        lastPath.erase(lastPath.begin());
        
        // Update heat map for visualization
        updateHeatMapForPath(snake, food, lastPath);
        return nextDir;
    }

    // If no path found, fallback to closest safe direction
    Direction bestDir = snake.getCurrentDirection();
    float bestScore = -std::numeric_limits<float>::max();
    sf::Vector2i head = snake.getHead();

    for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        if (!isMoveSafe(dir, snake)) continue;
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        float score = -getManhattanDistance(Position(nextPos), Position(food));
        if (score > bestScore) {
            bestScore = score;
            bestDir = dir;
        }
    }
    
    return bestDir;
}

void AdvancedStrategy::update() {
    // No update needed for now
}

std::vector<Direction> AdvancedStrategy::findPathToFood(const Snake& snake, const sf::Vector2i& food) {
    std::priority_queue<
        std::pair<int, std::pair<Position, std::vector<Direction>>>,
        std::vector<std::pair<int, std::pair<Position, std::vector<Direction>>>>,
        std::greater<>
    > openSet;
    
    std::set<Position> closedSet;
    Position start(snake.getHead());
    
    // Initialize with starting position
    openSet.push({0, {start, std::vector<Direction>()}});
    
    while (!openSet.empty()) {
        auto [fScore, current] = openSet.top();
        auto [pos, path] = current;
        openSet.pop();
        
        if (pos.pos == food) {
            return path;
        }
        
        if (closedSet.count(pos)) {
            continue;
        }
        closedSet.insert(pos);
        
        // Try all directions
        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = pos;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
            }
            
            if (closedSet.count(next) || !isMoveSafe(dir, snake)) {
                continue;
            }
            
            auto nextPath = path;
            nextPath.push_back(dir);
            
            int g = nextPath.size();
            int h = getManhattanDistance(next, Position(food));
            int f = g + h;
            
            openSet.push({f, {next, nextPath}});
        }
    }
    
    return std::vector<Direction>();
}

void AdvancedStrategy::updateHeatMapForPath(const Snake& snake, const sf::Vector2i& food, 
                                          const std::vector<Direction>& path) {
    // Clear heat map
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            BaseStrategy::heatMap.setValue(x, y, 0);  // Access through base class
        }
    }
    
    // Highlight the path
    sf::Vector2i pos = snake.getHead();
    float value = 1.0f;
    
    // Set higher values for the calculated path
    for (const auto& dir : path) {
        switch (dir) {
            case Direction::Up:    pos.y--; break;
            case Direction::Down:  pos.y++; break;
            case Direction::Left:  pos.x--; break;
            case Direction::Right: pos.x++; break;
        }
        BaseStrategy::heatMap.setValue(pos.x, pos.y, value);
        value *= 0.9f; // Gradient effect along the path
    }
    
    BaseStrategy::heatMap.normalize();
}

float AdvancedStrategy::calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const {
    Position pos(x, y);
    float score = 0.0f;
    
    // Distance to food
    score -= getManhattanDistance(pos, Position(food)) * 2.0f;
    
    // Wall proximity penalty
    if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
        y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
        score -= 10.0f;
    }
    
    // Only calculate accessible space for moves we're actually considering
    int space = countAccessibleSpace(pos, snake);
    score += space * 0.5f;
    
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

void AdvancedStrategy::updateHeatMap(const Snake& snake, const sf::Vector2i& food) {
    scoreCache.assign(GameConfig::GRID_WIDTH, 
        std::vector<float>(GameConfig::GRID_HEIGHT, -std::numeric_limits<float>::max()));
    
    // Only calculate scores for accessible positions
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            Position pos(x, y);
            if (!isPositionBlocked(pos, snake)) {
                float score = calculatePositionScore(x, y, snake, food);
                scoreCache[x][y] = score;
                heatMap.setValue(x, y, score);
            }
        }
    }
    
    heatMap.normalize();
}