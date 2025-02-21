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
    
    // Calculate scores in a gradient pattern
    for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
            Position pos{sf::Vector2i(x, y)};
            
            // Exponentially decaying base score from distance to food
            float foodDistance = getManhattanDistance(pos, Position{food});
            float score = 150.0f * std::exp(-foodDistance / 8.0f);
            
            // Add exponential bonus for being near the head
            float headDistance = getManhattanDistance(pos, Position{head});
            if (headDistance <= VIEW_RADIUS) {
                score += 100.0f * std::exp(-headDistance / 3.0f);
            }
            
            // Penalties and adjustments
            if (isPositionBlocked(pos, snake)) {
                score = -100.0f;  // More negative to create contrast
            } else if (x == 0 || x == GameConfig::GRID_WIDTH - 1 || 
                      y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                score *= 0.3f;  // Stronger wall penalty
            }
            
            // Only set scores above threshold
            if (score > 5.0f) {  // Increased minimum threshold
                gridHeatMap.setValue(x, y, score);
            }
        }
    }
    
    // Highlight the path with stronger contrast
    if (!lastPath.empty()) {
        sf::Vector2i pathPos = head;
        float pathScore = 500.0f;  // Increased initial path score
        
        gridHeatMap.setValue(pathPos.x, pathPos.y, pathScore);
        
        for (const auto& dir : lastPath) {
            switch (dir) {
                case Direction::Up:    pathPos.y--; break;
                case Direction::Down:  pathPos.y++; break;
                case Direction::Left:  pathPos.x--; break;
                case Direction::Right: pathPos.x++; break;
            }
            pathScore *= 0.80f;  // Steeper falloff for more contrast
            if (pathPos.x >= 0 && pathPos.x < GameConfig::GRID_WIDTH &&
                pathPos.y >= 0 && pathPos.y < GameConfig::GRID_HEIGHT) {
                gridHeatMap.setValue(pathPos.x, pathPos.y, pathScore);
            }
        }
    }
    
    // Emphasize important positions with higher values
    gridHeatMap.setValue(food.x, food.y, 600.0f);  // Increased food visibility
    gridHeatMap.setValue(head.x, head.y, 550.0f);  // Increased head visibility
    
    // Add stronger highlight around food
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int fx = food.x + dx;
            int fy = food.y + dy;
            if (fx >= 0 && fx < GameConfig::GRID_WIDTH && 
                fy >= 0 && fy < GameConfig::GRID_HEIGHT) {
                float currentScore = gridHeatMap.getValue(fx, fy);
                gridHeatMap.setValue(fx, fy, std::max(currentScore, 300.0f));
            }
        }
    }
    
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