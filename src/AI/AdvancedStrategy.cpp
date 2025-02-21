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
    // Early exit if path is too long
    if (getManhattanDistance(Position(snake.getHead()), Position(food)) > MAX_PATH_LENGTH) {
        return std::vector<Direction>();
    }
    
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

    // Add timeout check:
    while (!openSet.empty() && iterations++ < MAX_ITERATIONS) {
        if (pathfindClock.getElapsedTime().asMilliseconds() > PATHFINDING_TIMEOUT_MS) {
            return std::vector<Direction>();  // Abort if taking too long
        }
        
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
    static sf::Clock updateClock;
    if (updateClock.getElapsedTime().asMilliseconds() < 50) {
        return;
    }
    updateClock.restart();

    gridHeatMap.clear();
    sf::Vector2i head = snake.getHead();
    
    // Base heat map with stronger gradient
    for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
            Position pos{sf::Vector2i(x, y)};
            
            // Calculate distance-based score with stronger falloff
            float foodDistance = getManhattanDistance(pos, Position{food});
            float headDistance = getManhattanDistance(pos, Position{head});
            
            // Quadratic falloff for better gradient
            float score = 200.0f / (1.0f + (foodDistance * foodDistance / 100.0f));
            
            // Add head proximity bonus with cubic falloff
            if (headDistance <= VIEW_RADIUS) {
                score += 150.0f / (1.0f + (headDistance * headDistance * headDistance / 100.0f));
            }
            
            // Penalties
            if (isPositionBlocked(pos, snake)) {
                score = -50.0f;  // Snake body
            } else if (x == 0 || x == GameConfig::GRID_WIDTH - 1 || 
                      y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                score *= 0.2f;  // Stronger wall penalty
            }
            
            // Only set scores above minimum threshold
            if (score > 1.0f) {
                // Use negative scores for base heat map (will be rendered as red gradient)
                gridHeatMap.setValue(x, y, -score);
            }
        }
    }
    
    // Mark planned path with positive values (will be rendered as blue gradient)
    if (!lastPath.empty()) {
        sf::Vector2i pathPos = head;
        float pathScore = 800.0f;  // Higher value for path
        
        for (const auto& dir : lastPath) {
            switch (dir) {
                case Direction::Up:    pathPos.y--; break;
                case Direction::Down:  pathPos.y++; break;
                case Direction::Left:  pathPos.x--; break;
                case Direction::Right: pathPos.x++; break;
            }
            pathScore *= 0.85f;  // Steeper falloff
            if (pathPos.x >= 0 && pathPos.x < GameConfig::GRID_WIDTH &&
                pathPos.y >= 0 && pathPos.y < GameConfig::GRID_HEIGHT) {
                // Use positive scores for path (will be rendered as blue)
                gridHeatMap.setValue(pathPos.x, pathPos.y, pathScore);
            }
        }
    }
    
    // Special markers for key positions (will be rendered as yellow)
    gridHeatMap.setValue(food.x, food.y, 1000.0f);  // Food marker
    gridHeatMap.setValue(head.x, head.y, 900.0f);   // Head marker
    
    gridHeatMap.triggerUpdate();
}

float AdvancedStrategy::calculateSimpleScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const {
    Position pos{sf::Vector2i(x, y)};
    
    if (x == food.x && y == food.y) return 200.0f;
    if (isPositionBlocked(pos, snake)) return 0.0f;

    // Simpler scoring based just on distance
    return 100.0f / (1.0f + getManhattanDistance(pos, Position{food}));
}

float AdvancedStrategy::calculatePositionScore(int x, int y, const Snake& snake, const sf::Vector2i& food) const {
    Position pos{sf::Vector2i(x, y)};
    
    // High score for food position
    if (x == food.x && y == food.y) {
        return 200.0f;
    }

    // Zero score for blocked positions
    if (isPositionBlocked(pos, snake)) {
        return 0.0f;
    }

    // Base score is inverse distance to food
    float distanceScore = 100.0f / (1.0f + getManhattanDistance(pos, Position{food}));

    // Add path-finding component
    float accessSpace = static_cast<float>(countAccessibleSpace(pos, snake));
    float spaceScore = accessSpace / (GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT) * 50.0f;

    return distanceScore + spaceScore;
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