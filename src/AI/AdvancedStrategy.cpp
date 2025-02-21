#include "AI/AdvancedStrategy.hpp"
#include "GameConfig.hpp"
#include <queue>
#include <map>
#include <set>
#include <iostream>

Direction AdvancedStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    // Update position tracking
    sf::Vector2i head = snake.getHead();
    if (head == lastPosition) {
        stuckCount++;
        if (stuckCount > 3) {
            stuckCount = 0;
            return calculateRandomMove(snake);
        }
    } else {
        stuckCount = 0;
        lastPosition = head;
    }

    // Try to use existing path first
    if (!lastPath.empty()) {
        Direction nextDir = lastPath.front();
        if (isMoveSafe(nextDir, snake)) {
            lastPath.erase(lastPath.begin());
            return nextDir;
        }
        lastPath.clear();
    }

    // Find new path to food
    lastPath = findPathToFood(snake, food);
    if (!lastPath.empty()) {
        Direction nextDir = lastPath.front();
        lastPath.erase(lastPath.begin());
        return nextDir;
    }

    // If no path to food, evaluate immediate neighbors only
    float bestScore = -std::numeric_limits<float>::max();
    Direction bestDir = snake.getCurrentDirection();
    
    for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        if (!isMoveSafe(dir, snake)) continue;
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        // Only calculate score for immediate moves
        float score = calculatePositionScore(nextPos.x, nextPos.y, snake, food);
        
        // Update heat map for visualization
        heatMap.setValue(nextPos.x, nextPos.y, score);
        
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
    Position start(snake.getHead());
    Position goal(food);
    
    // Priority queue ordered by f-score (g + h)
    auto compareScores = [this, &food](const std::pair<Position, std::vector<Direction>>& a,
                                     const std::pair<Position, std::vector<Direction>>& b) {
        int g_a = a.second.size(); // Path length so far
        int h_a = this->getManhattanDistance(a.first, Position(food)); // Heuristic
        int g_b = b.second.size();
        int h_b = this->getManhattanDistance(b.first, Position(food));
        return (g_a + h_a) > (g_b + h_b); // Priority queue is max-heap by default
    };
    
    std::priority_queue<
        std::pair<Position, std::vector<Direction>>,
        std::vector<std::pair<Position, std::vector<Direction>>>,
        decltype(compareScores)
    > openSet(compareScores);
    
    std::set<Position> closedSet;
    openSet.push({start, std::vector<Direction>()});
    
    int maxIterations = GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT;
    int iterations = 0;
    
    while (!openSet.empty() && iterations < maxIterations) {
        iterations++;
        auto [current, path] = openSet.top();
        openSet.pop();
        
        if (current.pos == food) {
            std::cout << "Path found to food in " << iterations << " iterations" << std::endl;
            return path;
        }
        
        if (closedSet.count(current)) {
            continue;
        }
        closedSet.insert(current);
        
        // Consider each possible direction
        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = current;
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
            openSet.push({next, nextPath});
        }
    }
    
    std::cout << "No path found after " << iterations << " iterations" << std::endl;
    return std::vector<Direction>();
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