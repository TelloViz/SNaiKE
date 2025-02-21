#include "AI/AdvancedStrategy.hpp"
#include "GameConfig.hpp"
#include <queue>
#include <map>
#include <set>
#include <iostream>

Direction AdvancedStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    // Try to find path to food first using A*
    lastPath = findPathToFood(snake, food);
    
    // Update heat map based on current state
    updateHeatMapVisualization(snake, food);
    
    if (!lastPath.empty()) {
        Direction nextDir = lastPath.front();
        lastPath.erase(lastPath.begin());
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
    static sf::Clock pathfindClock;
    static Position lastFoodPos;
    
    // Cache check for performance
    if (lastFoodPos == Position(food) && 
        !lastPath.empty() && 
        pathfindClock.getElapsedTime().asMilliseconds() < 100) {
        return lastPath;
    }
    
    lastFoodPos = Position(food);
    pathfindClock.restart();

    // A* implementation with priority queue
    std::priority_queue<
        std::pair<int, std::pair<Position, std::vector<Direction>>>,
        std::vector<std::pair<int, std::pair<Position, std::vector<Direction>>>>,
        std::greater<>
    > openSet;
    
    std::set<Position> closedSet;
    std::map<Position, int> gScore;
    
    Position start(snake.getHead());
    Position goal(food);
    
    openSet.push({0, {start, std::vector<Direction>()}});
    gScore[start] = 0;
    
    int maxIterations = GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT;
    int iterations = 0;

    while (!openSet.empty() && iterations++ < maxIterations) {
        auto [fScore, current] = openSet.top();
        auto [pos, path] = current;
        openSet.pop();
        
        if (pos.pos == food) {
            lastPath = path;
            return path;
        }
        
        if (closedSet.count(pos)) continue;
        closedSet.insert(pos);
        
        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = pos;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
            }
            
            if (closedSet.count(next) || !isMoveSafe(dir, snake)) continue;
            
            int tentativeG = gScore[pos] + 1;
            if (gScore.find(next) == gScore.end() || tentativeG < gScore[next]) {
                auto nextPath = path;
                nextPath.push_back(dir);
                gScore[next] = tentativeG;
                
                int h = getManhattanDistance(next, Position(food));
                openSet.push({tentativeG + h, {next, nextPath}});
            }
        }
    }
    
    return std::vector<Direction>();
}

void AdvancedStrategy::updateHeatMapVisualization(const Snake& snake, const sf::Vector2i& food) {
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            Position pos(x, y);
            float score = 0.0f;
            
            // Base score on distance to food
            score -= BaseStrategy::getManhattanDistance(pos, Position(food)) * 0.5f;
            
            // Add extra score for positions on the current path
            if (!lastPath.empty()) {
                sf::Vector2i pathPos = snake.getHead();
                for (const auto& dir : lastPath) {
                    if (pos.pos == pathPos) {
                        score += 50.0f;
                        break;
                    }
                    switch (dir) {
                        case Direction::Up:    pathPos.y--; break;
                        case Direction::Down:  pathPos.y++; break;
                        case Direction::Left:  pathPos.x--; break;
                        case Direction::Right: pathPos.x++; break;
                    }
                }
            }
            
            // Penalties
            if (BaseStrategy::isPositionBlocked(pos, snake)) {
                score = -100.0f;
            }
            else if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
                     y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                score -= 10.0f;
            }
            
            this->heatMap.setValue(x, y, score);
        }
    }
    
    // Set food position to high value for visualization
    this->heatMap.setValue(food.x, food.y, 100.0f);
    this->heatMap.normalize();
}

void AdvancedStrategy::updateHeatMapForPath(const Snake& snake, const sf::Vector2i& food, 
                                          const std::vector<Direction>& path) {
    // Clear heat map
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            // Set negative values for non-path cells
            BaseStrategy::heatMap.setValue(x, y, -1.0f);
        }
    }
    
    // Highlight the path with positive values
    sf::Vector2i pos = snake.getHead();
    float value = 1.0f;
    
    // Set higher values for the calculated path
    BaseStrategy::heatMap.setValue(pos.x, pos.y, value);
    for (const auto& dir : path) {
        switch (dir) {
            case Direction::Up:    pos.y--; break;
            case Direction::Down:  pos.y++; break;
            case Direction::Left:  pos.x--; break;
            case Direction::Right: pos.x++; break;
        }
        value = std::max(0.3f, value * 0.9f); // Keep minimum visibility
        BaseStrategy::heatMap.setValue(pos.x, pos.y, value);
    }
    
    // Set food position to maximum value
    BaseStrategy::heatMap.setValue(food.x, food.y, 1.0f);
    
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