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
    // Keep existing path if it's still valid
    if (!lastPath.empty()) {
        Position nextPos = Position(snake.getHead());
        bool pathIsValid = true;
        
        // Verify current path is still valid
        for (const Direction& dir : lastPath) {
            switch (dir) {
                case Direction::Up:    nextPos.pos.y--; break;
                case Direction::Down:  nextPos.pos.y++; break;
                case Direction::Left:  nextPos.pos.x--; break;
                case Direction::Right: nextPos.pos.x++; break;
            }
            if (isPositionBlocked(nextPos, snake)) {
                pathIsValid = false;
                break;
            }
        }
        
        if (pathIsValid) return lastPath;
    }

    // Early exit if path is too long
    if (this->getManhattanDistance(Position(snake.getHead()), Position(food)) > MAX_PATH_LENGTH) {
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

            // Enhanced safety check
            if (closedSet.count(next) || 
                !this->isMoveSafe(dir, snake) || 
                isPositionBlocked(next, snake)) continue;

            int tentativeG = gScore[current] + 1;
            
            if (gScore.find(next) == gScore.end() || tentativeG < gScore[next]) {
                cameFrom[next] = current;
                directionToParent[next] = dir;
                gScore[next] = tentativeG;
                int h = this->getManhattanDistance(next, goal);
                openSet.push({tentativeG + h, next});
            }
        }
    }

    return std::vector<Direction>();
}

void AdvancedStrategy::updateHeatMapVisualization(const Snake& snake, const sf::Vector2i& food) {
    static sf::Clock updateClock;
    if (updateClock.getElapsedTime().asMilliseconds() < 50) return;
    updateClock.restart();

    gridHeatMap.clear();

    // Visualize current committed path in orange
    if (!lastPath.empty()) {
        sf::Vector2i pathPos = snake.getHead();
        for (const Direction& dir : lastPath) {
            switch (dir) {
                case Direction::Up:    pathPos.y--; break;
                case Direction::Down:  pathPos.y++; break;
                case Direction::Left:  pathPos.x--; break;
                case Direction::Right: pathPos.x++; break;
            }
            if (isValidPosition(pathPos)) {
                gridHeatMap.setValue(pathPos.x, pathPos.y, 850.0f); // Orange
            }
        }
    }

    // Visualize exploration paths in magenta
    std::vector<std::vector<Direction>> explorationPaths = findAlternativePaths(snake, food);
    for (const auto& path : explorationPaths) {
        sf::Vector2i pathPos = snake.getHead();
        float score = 700.0f; // Start value for magenta paths
        for (const Direction& dir : path) {
            switch (dir) {
                case Direction::Up:    pathPos.y--; break;
                case Direction::Down:  pathPos.y++; break;
                case Direction::Left:  pathPos.x--; break;
                case Direction::Right: pathPos.x++; break;
            }
            if (isValidPosition(pathPos)) {
                // Only set if not part of committed path
                if (gridHeatMap.getValue(pathPos.x, pathPos.y) < 800.0f) {
                    gridHeatMap.setValue(pathPos.x, pathPos.y, score);
                }
                score *= 0.95f; // Fade out exploration paths
            }
        }
    }

    // Mark critical positions
    gridHeatMap.setValue(food.x, food.y, 1000.0f);    // Food
    gridHeatMap.setValue(snake.getHead().x, snake.getHead().y, 900.0f);  // Head
}

std::vector<std::vector<Direction>> AdvancedStrategy::findAlternativePaths(
    const Snake& snake, const sf::Vector2i& food) {
    std::vector<std::vector<Direction>> paths;
    
    // Try different initial directions
    for (Direction startDir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        if (!isMoveSafe(startDir, snake)) continue;

        // Quick path search from each possible start direction
        Position start = Position(snake.getHead());
        switch (startDir) {
            case Direction::Up:    start.pos.y--; break;
            case Direction::Down:  start.pos.y++; break;
            case Direction::Left:  start.pos.x--; break;
            case Direction::Right: start.pos.x++; break;
        }

        if (!isPositionBlocked(start, snake)) {
            std::vector<Direction> path = quickPathFind(start, Position(food), snake);
            if (!path.empty()) {
                path.insert(path.begin(), startDir);
                paths.push_back(path);
            }
        }
    }
    
    return paths;
}

void AdvancedStrategy::updateHeatMap(const Snake& snake, const sf::Vector2i& food) {
    static sf::Clock updateClock;
    if (updateClock.getElapsedTime().asMilliseconds() < 50) return;
    updateClock.restart();

    gridHeatMap.clear();
    sf::Vector2i head = snake.getHead();
    Direction currentDir = snake.getCurrentDirection();

    // Calculate scores for visible area only
    const int VIEW_RADIUS = 6;  // Reduced from 8
    for (int x = std::max(0, head.x - VIEW_RADIUS); 
         x <= std::min(GameConfig::GRID_WIDTH - 1, head.x + VIEW_RADIUS); ++x) {
        for (int y = std::max(0, head.y - VIEW_RADIUS); 
             y <= std::min(GameConfig::GRID_HEIGHT - 1, head.y + VIEW_RADIUS); ++y) {
            Position pos{sf::Vector2i(x, y)};
            float headDistance = getManhattanDistance(pos, Position{head});
            if (headDistance > VIEW_RADIUS) continue;

            float score = 0.0f;
            
            if (isPositionBlocked(pos, snake)) {
                score = -500.0f;
            } else {
                // Food influence
                float foodDistance = getManhattanDistance(pos, Position{food});
                score += 200.0f / (1.0f + foodDistance);

                // Only calculate space for immediate moves
                if (headDistance <= 2) {  // Reduced from 3
                    int space = countAccessibleSpace(pos, snake);
                    float spaceRatio = static_cast<float>(space) / 
                                     (GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT);
                    score += spaceRatio * 300.0f;
                }

                // Direction preference
                sf::Vector2i dirVec;
                switch (currentDir) {
                    case Direction::Up:    dirVec = {0, -1}; break;
                    case Direction::Down:  dirVec = {0, 1}; break;
                    case Direction::Left:  dirVec = {-1, 0}; break;
                    case Direction::Right: dirVec = {1, 0}; break;
                }
                float dirScore = (dirVec.x * (x - head.x) + dirVec.y * (y - head.y)) * 5.0f;
                score += dirScore;
            }

            if (std::abs(score) > 0.1f) {
                gridHeatMap.setValue(x, y, score);
            }
        }
    }

    // Cache and update path
    if (pathUpdateClock.getElapsedTime().asMilliseconds() > 100) {  // Reduced frequency
        lastPath = findPathToFood(snake, food);
        pathUpdateClock.restart();
    }

    // Visualize path
    if (!lastPath.empty()) {
        sf::Vector2i pathPos = head;
        float pathScore = 800.0f;
        for (const Direction& dir : lastPath) {
            switch (dir) {
                case Direction::Up:    pathPos.y--; break;
                case Direction::Down:  pathPos.y++; break;
                case Direction::Left:  pathPos.x--; break;
                case Direction::Right: pathPos.x++; break;
            }
            if (pathPos.x >= 0 && pathPos.x < GameConfig::GRID_WIDTH &&
                pathPos.y >= 0 && pathPos.y < GameConfig::GRID_HEIGHT) {
                gridHeatMap.setValue(pathPos.x, pathPos.y, pathScore);
                pathScore *= 0.9f;
            }
        }
    }

    // Mark critical positions
    gridHeatMap.setValue(food.x, food.y, 1000.0f);
    gridHeatMap.setValue(head.x, head.y, 900.0f);

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

// Add this implementation before the other member functions:
void AdvancedStrategy::render(sf::RenderWindow& window) const {
    gridHeatMap.render(window, sf::Vector2f(GameConfig::CELL_SIZE, GameConfig::CELL_SIZE));
}

std::vector<Direction> AdvancedStrategy::quickPathFind(
    const Position& start, const Position& goal, const Snake& snake) const {
    
    std::set<Position> visited;
    std::map<Position, Position> cameFrom;
    std::map<Position, Direction> directionToParent;
    std::queue<Position> queue;
    
    queue.push(start);
    visited.insert(start);
    
    while (!queue.empty()) {
        Position current = queue.front();
        queue.pop();
        
        if (current == goal) {
            // Reconstruct path
            std::vector<Direction> path;
            Position currentPos = current;
            
            while (currentPos != start) {
                path.push_back(directionToParent[currentPos]);
                currentPos = cameFrom[currentPos];
            }
            
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        // Try all four directions
        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = current;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
            }
            
            if (!visited.count(next) && !isPositionBlocked(next, snake)) {
                queue.push(next);
                visited.insert(next);
                cameFrom[next] = current;
                directionToParent[next] = dir;
            }
        }
    }
    
    return std::vector<Direction>();  // No path found
}