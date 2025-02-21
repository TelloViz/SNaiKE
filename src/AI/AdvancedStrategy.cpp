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

    Position start(snake.getHead());
    Position goal(food);

    // More efficient implementation using only what we need
    std::set<Position> closedSet;
    std::map<Position, Position> cameFrom;
    std::map<Position, int> gScore;
    std::map<Position, Direction> directionToParent;
    
    // Priority queue using int for f-score (simpler, faster)
    std::priority_queue<std::pair<int, Position>, 
        std::vector<std::pair<int, Position>>, 
        std::greater<>> openSet;
    
    openSet.push({0, start});
    gScore[start] = 0;

    int iterations = 0;
    const int MAX_ITERATIONS = 1000; // Reasonable limit

    while (!openSet.empty() && iterations++ < MAX_ITERATIONS) {
        Position current = openSet.top().second;
        openSet.pop();

        if (current == goal) {
            // Reconstruct path
            std::vector<Direction> path;
            Position currentPos = current;
            
            while (currentPos != start) {
                path.push_back(directionToParent[currentPos]);
                currentPos = cameFrom[currentPos];
            }
            
            std::reverse(path.begin(), path.end());
            lastPath = path;
            return path;
        }

        if (closedSet.count(current)) continue;
        closedSet.insert(current);

        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = current;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
            }

            if (closedSet.count(next) || !isMoveSafe(dir, snake)) continue;

            int tentativeG = gScore[current] + 1;
            
            if (gScore.find(next) == gScore.end() || tentativeG < gScore[next]) {
                cameFrom[next] = current;
                directionToParent[next] = dir;
                gScore[next] = tentativeG;
                int h = getManhattanDistance(next, goal);
                openSet.push({tentativeG + h, next});
            }
        }
    }

    return std::vector<Direction>();
}

void AdvancedStrategy::updateHeatMapVisualization(const Snake& snake, const sf::Vector2i& food) {
    const int RENDER_RADIUS = 8;
    sf::Vector2i head = snake.getHead();
    
    // Clear all scores first
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            this->heatMap.setValue(x, y, -100.0f);
        }
    }

    // Only process cells within radius of snake head
    for (int x = std::max(0, head.x - RENDER_RADIUS); 
         x < std::min(GameConfig::GRID_WIDTH, head.x + RENDER_RADIUS); x++) {
        for (int y = std::max(0, head.y - RENDER_RADIUS);
             y < std::min(GameConfig::GRID_HEIGHT, head.y + RENDER_RADIUS); y++) {
            
            Position pos(x, y);
            float score = 0.0f;
            
            if (BaseStrategy::isPositionBlocked(pos, snake)) {
                score = -100.0f;
            } else {
                // Distance to food
                score -= getManhattanDistance(pos, Position(food)) * 3.0f;
                
                // Only calculate space for immediate next moves
                bool isNextMove = false;
                for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
                    sf::Vector2i nextPos = head;
                    switch (dir) {
                        case Direction::Up:    nextPos.y--; break;
                        case Direction::Down:  nextPos.y++; break;
                        case Direction::Left:  nextPos.x--; break;
                        case Direction::Right: nextPos.x++; break;
                    }
                    if (pos.pos == nextPos) {
                        isNextMove = true;
                        break;
                    }
                }
                
                // Only calculate accessible space for next possible moves
                if (isNextMove) {
                    int space = countAccessibleSpace(pos, snake);
                    score += space * 0.8f;
                }
                
                // Distance from head bonus
                float distToHead = getManhattanDistance(pos, Position(head));
                score += (RENDER_RADIUS - distToHead) * 2.0f;
                
                // Wall penalties
                if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
                    y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                    score -= 30.0f;
                }
                
                // Corner penalties
                if ((x == 0 || x == GameConfig::GRID_WIDTH - 1) &&
                    (y == 0 || y == GameConfig::GRID_HEIGHT - 1)) {
                    score -= 50.0f;
                }
                
                // Add morphing effect
                float time = static_cast<float>(gameTimer.getElapsedTime().asSeconds());
                float oscillation = std::sin(time * 2.0f + (x + y) * 0.5f) * 10.0f;
                score += oscillation;
            }
            
            this->heatMap.setValue(x, y, score);
        }
    }
    
    // Mark food
    this->heatMap.setValue(food.x, food.y, 200.0f);
    
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