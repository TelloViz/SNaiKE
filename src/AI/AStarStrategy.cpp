#include "AI/AStarStrategy.hpp"
#include <queue>
#include <iostream>

void AStarStrategy::render(sf::RenderWindow& window) const {
    // Heat map rendering using global state
    if (globalShowHeatMap) {
        sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE - 2));
        for (const auto& node : exploredNodes) {
            float heat = static_cast<float>(
                std::distance(exploredNodes.begin(), 
                std::find(exploredNodes.begin(), exploredNodes.end(), node))
            ) / exploredNodes.size();
            
            cell.setFillColor(sf::Color(
                static_cast<sf::Uint8>(255 * heat),
                0,
                static_cast<sf::Uint8>(255 * (1-heat)),
                96
            ));
            
            cell.setPosition(
                node.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + 1,
                node.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + 1
            );
            window.draw(cell);
        }
    }

    // Path arrow rendering stays local
    if (showPathArrows && !visualPath.empty()) {
        sf::Vector2i pos = snake.getHead();
        for (const Direction& dir : visualPath) {
            sf::ConvexShape arrow;
            arrow.setPointCount(3);
            arrow.setFillColor(sf::Color(0, 255, 0, 180)); // Semi-transparent green

            float arrowSize = GameConfig::CELL_SIZE * 0.4f;
            switch (dir) {
                case Direction::Up:
                    arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE/2, 2));
                    arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE/2 - arrowSize, arrowSize + 2));
                    arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE/2 + arrowSize, arrowSize + 2));
                    break;
                case Direction::Down:
                    arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE/2, GameConfig::CELL_SIZE - 2));
                    arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE/2 - arrowSize, GameConfig::CELL_SIZE - arrowSize - 2));
                    arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE/2 + arrowSize, GameConfig::CELL_SIZE - arrowSize - 2));
                    break;
                case Direction::Left:
                    arrow.setPoint(0, sf::Vector2f(2, GameConfig::CELL_SIZE/2));
                    arrow.setPoint(1, sf::Vector2f(arrowSize + 2, GameConfig::CELL_SIZE/2 - arrowSize));
                    arrow.setPoint(2, sf::Vector2f(arrowSize + 2, GameConfig::CELL_SIZE/2 + arrowSize));
                    break;
                case Direction::Right:
                    arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE/2));
                    arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE - arrowSize - 2, GameConfig::CELL_SIZE/2 - arrowSize));
                    arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE - arrowSize - 2, GameConfig::CELL_SIZE/2 + arrowSize));
                    break;
            }

            arrow.setPosition(
                pos.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES,
                pos.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP
            );
            window.draw(arrow);

            // Update position for next arrow
            switch (dir) {
                case Direction::Up:    pos.y--; break;
                case Direction::Down:  pos.y++; break;
                case Direction::Left:  pos.x--; break;
                case Direction::Right: pos.x++; break;
            }
        }
    }
}

AStarStrategy::AStarStrategy(const Snake& snakeRef) 
    : snake(snakeRef)
    , currentHeuristic(Heuristic::MANHATTAN)
    , showPathArrows(false)
    , hasExplorationData(false)
{
    // Reset clocks
    renderClock.restart();
    explorationRenderClock.restart();
    pathUpdateClock.restart();
}

Direction AStarStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    if (currentPath.empty() || pathUpdateClock.getElapsedTime().asSeconds() >= PATH_UPDATE_INTERVAL) {
        currentPath = findPath(snake, food);
        visualPath = currentPath;  // Store a copy for visualization
        pathUpdateClock.restart();
    }

    if (!currentPath.empty()) {
        Direction nextMove = currentPath.front();
        currentPath.erase(currentPath.begin());
        return nextMove;
    }
    return snake.getCurrentDirection();
}

std::vector<Direction> AStarStrategy::findPath(const Snake& snake, const sf::Vector2i& food) {
    Position start{snake.getHead()};  // Use uniform initialization
    Position goal{food};              // Use uniform initialization
    
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::map<Position, Position> cameFrom;
    std::map<Position, float> gScore;
    std::map<Position, Direction> directionToParent;

    openSet.push({start, 0, calculateHeuristic(start, food)});
    gScore[start] = 0;

    exploredNodes.clear();  // Clear previous exploration data

    while (!openSet.empty()) {
        Position current = openSet.top().pos;
        openSet.pop();

        // Always collect explored nodes regardless of heat map state
        exploredNodes.push_back(current.pos);

        if (current == goal) {
            // Found path to food, now verify it's safe
            std::vector<Direction> path = reconstructPath(start, goal, cameFrom, directionToParent);
            if (isPathSafe(path, snake)) {
                return path;
            }
            continue; // Path wasn't safe, keep searching
        }

        for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            Position next = current;
            switch (dir) {
                case Direction::Up:    next.pos.y--; break;
                case Direction::Down:  next.pos.y++; break;
                case Direction::Left:  next.pos.x--; break;
                case Direction::Right: next.pos.x++; break;
            }

            if (!isPositionSafe(next, snake)) continue;

            float moveCost = 1.0f;

            // Heavy penalty for crossing snake's body
            const auto& body = snake.getBody();
            for (size_t i = 0; i < body.size(); i++) {
                float distToSegment = getManhattanDistance(next.pos, body[i]);
                if (distToSegment < 3.0f) {  // Increased detection range
                    moveCost += 20.0f / (distToSegment + 1.0f);  // Increased penalty
                }
            }

            // Extreme penalty for direction reversals
            Direction currentDir = snake.getCurrentDirection();
            if ((dir == Direction::Up && currentDir == Direction::Down) ||
                (dir == Direction::Down && currentDir == Direction::Up) ||
                (dir == Direction::Left && currentDir == Direction::Right) ||
                (dir == Direction::Right && currentDir == Direction::Left)) {
                moveCost += 50.0f;  // Much higher penalty
            }

            // Penalty for moving towards own body
            if (isMovingTowardsBody(next, dir, snake)) {
                moveCost += 15.0f;
            }

            float tentativeGScore = gScore[current] + moveCost;
            if (!gScore.count(next) || tentativeGScore < gScore[next]) {
                cameFrom[next] = current;
                gScore[next] = tentativeGScore;
                directionToParent[next] = dir;
                
                float hScore = calculateHeuristic(next, food);
                openSet.push({next, tentativeGScore, hScore});
            }
        }
    }

    return {};
}

float AStarStrategy::calculateHeuristic(const Position& pos, const sf::Vector2i& goal) const {
    switch(currentHeuristic) {
        case Heuristic::MANHATTAN:
            return getManhattanDistance(pos, goal);
        case Heuristic::EUCLIDEAN:
            return std::sqrt(
                std::pow(pos.pos.x - goal.x, 2) + 
                std::pow(pos.pos.y - goal.y, 2)
            );
        case Heuristic::CHEBYSHEV:
            return std::max(
                std::abs(pos.pos.x - goal.x),
                std::abs(pos.pos.y - goal.y)
            );
        default:
            return getManhattanDistance(pos, goal);
    }
}

void AStarStrategy::update() {
    // Remove the conditional checks - we want to always update both independently
    if (!exploredNodes.empty()) {
        lastExploredNodes = exploredNodes;
        hasExplorationData = true;
    }
    
    // Always update visual path, regardless of showPathArrows state
    visualPath = currentPath;
}

int AStarStrategy::countAccessibleSpace(const Position& start, const Snake& snake) const {
    std::vector<Position> visited;
    std::queue<Position> toVisit;
    toVisit.push(start);
    
    while (!toVisit.empty()) {
        Position current = toVisit.front();
        toVisit.pop();
        
        if (std::find(visited.begin(), visited.end(), current) != visited.end()) {
            continue;
        }
        
        visited.push_back(current);
        
        for (const auto& neighbor : getNeighbors(current)) {
            if (neighbor.pos.x >= 0 && neighbor.pos.x < GameConfig::GRID_WIDTH &&
                neighbor.pos.y >= 0 && neighbor.pos.y < GameConfig::GRID_HEIGHT) {
                
                bool isBlocked = false;
                for (const auto& segment : snake.getBody()) {
                    if (neighbor.pos == segment) {
                        isBlocked = true;
                        break;
                    }
                }
                
                if (!isBlocked) {
                    toVisit.push(neighbor);
                }
            }
        }
    }
    
    return visited.size();
}

std::vector<Position> AStarStrategy::getNeighbors(const Position& pos) const {
    return {
        Position{pos.pos.x, pos.pos.y - 1},  // Up
        Position{pos.pos.x, pos.pos.y + 1},  // Down
        Position{pos.pos.x - 1, pos.pos.y},  // Left
        Position{pos.pos.x + 1, pos.pos.y}   // Right
    };
}

bool AStarStrategy::isPositionSafe(const Position& pos, const Snake& snake) const {
    // Check grid boundaries
    if (pos.pos.x < 0 || pos.pos.x >= GameConfig::GRID_WIDTH ||
        pos.pos.y < 0 || pos.pos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }

    const auto& body = snake.getBody();
    // Start checking from the third segment since first two can't be collided with
    for (size_t i = 2; i < body.size() - 1; ++i) {
        if (pos.pos == body[i]) {
            return false;
        }
    }

    // Only check accessible space if near middle segments
    bool nearBody = false;
    // Skip first two segments in this check as well
    for (size_t i = 2; i < body.size(); ++i) {
        if (getManhattanDistance(pos.pos, body[i]) < 2) {
            nearBody = true;
            break;
        }
    }

    if (nearBody && countAccessibleSpace(pos, snake) < body.size()) {
        return false;
    }

    return true;
}

bool AStarStrategy::isMovingTowardsBody(const Position& pos, Direction dir, const Snake& snake) const {
    const auto& body = snake.getBody();
    sf::Vector2i nextPos = pos.pos;
    
    // Only check 2 steps ahead instead of 3
    for (int steps = 1; steps <= 2; steps++) {
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        // Only check first few body segments
        for (size_t i = 0; i < std::min(size_t(5), body.size() - 1); i++) {
            if (BaseStrategy::getManhattanDistance(nextPos, body[i]) < steps) {
                return true;
            }
        }
    }
    return false;
}

bool AStarStrategy::isPathSafe(const std::vector<Direction>& path, const Snake& snake) const {
    if (path.empty()) return false;

    std::deque<sf::Vector2i> simSnakeBody = snake.getBody();
    sf::Vector2i simHead = simSnakeBody.front();

    for (Direction dir : path) {
        switch (dir) {
            case Direction::Up:    simHead.y--; break;
            case Direction::Down:  simHead.y++; break;
            case Direction::Left:  simHead.x--; break;
            case Direction::Right: simHead.x++; break;
        }

        // Check if this move would cause self-collision
        for (size_t i = 0; i < simSnakeBody.size() - 1; i++) {
            if (simHead == simSnakeBody[i]) return false;
        }

        simSnakeBody.pop_back();
        simSnakeBody.push_front(simHead);
    }
    return true;
}

std::vector<Direction> AStarStrategy::reconstructPath(
    const Position& start,
    const Position& goal,
    const std::map<Position, Position>& cameFrom,
    const std::map<Position, Direction>& directionToParent) const {
    
    std::vector<Direction> path;
    Position current = goal;

    while (current != start) {
        auto dirIt = directionToParent.find(current);
        if (dirIt == directionToParent.end()) {
            return {};  // Path reconstruction failed
        }
        path.push_back(dirIt->second);
        
        auto it = cameFrom.find(current);
        if (it == cameFrom.end()) {
            return {};  // Path reconstruction failed
        }
        current = it->second;
    }

    std::reverse(path.begin(), path.end());
    return path;
}

void AStarStrategy::togglePathArrows() {
    showPathArrows = !showPathArrows;
    std::cout << "Path arrows: " << (showPathArrows ? "ON" : "OFF") << std::endl;
}

// Add this implementation
void AStarStrategy::toggleHeatMap() {
    globalShowHeatMap = !globalShowHeatMap;
    std::cout << "Heat map: " << (globalShowHeatMap ? "ON" : "OFF") << std::endl;
}