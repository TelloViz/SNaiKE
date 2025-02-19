#include "AI/AIPlayer.hpp"
#include "GameConfig.hpp"
#include <iostream>

GameInput AIPlayer::getNextInput() {
    if (plannedMoves.empty()) {
        planNextMove();
    }
    
    if (!plannedMoves.empty()) {
        GameInput input = plannedMoves.front();
        plannedMoves.pop();
        return input;
    }
    
    return GameInput{InputType::ButtonPressed, GameButton::None};
}

void AIPlayer::planNextMove() {
    Direction targetDir = calculateDirectionToFood();
    
    // Convert direction to GameInput
    GameButton button = GameButton::None;
    switch (targetDir) {
        case Direction::Up:    button = GameButton::Up; break;
        case Direction::Down:  button = GameButton::Down; break;
        case Direction::Left:  button = GameButton::Left; break;
        case Direction::Right: button = GameButton::Right; break;
    }
    
    // Always push the move - isMoveSafe was already checked in calculateDirectionToFood
    if (button != GameButton::None) {
        std::cout << "AI planning move: " << static_cast<int>(button) << std::endl;  // Debug output
        plannedMoves.push(GameInput{InputType::ButtonPressed, button});
    }
}

Direction AIPlayer::calculateDirectionToFood() {
    switch (currentStrategy) {
        case AIStrategy::Basic:
            return calculateBasicMove();  // Current logic
        case AIStrategy::Advanced:
            return calculateAdvancedMove();
        case AIStrategy::Random:
            return calculateRandomMove();
        default:
            return calculateBasicMove();
    }
}

Direction AIPlayer::calculateAdvancedMove() {
    // First priority: Try to find a direct path to food
    auto path = findPathToFood();
    if (!path.empty()) {
        std::cout << "Found path to food of length: " << path.size() << std::endl;
        
        // Only do basic safety check for immediate move
        if (isMoveSafe(path[0])) {
            return path[0];
        }
    }

    // Fallback strategy: Use more basic approach but with space consideration
    const sf::Vector2i& head = snake.getHead();
    std::vector<std::pair<Direction, int>> possibleMoves;
    
    int currentDistanceToFood = getManhattanDistance(Position(head), Position(food));
    
    for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        if (!isMoveSafe(dir)) continue;  // Use simple safety check
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        int score = 0;
        
        // Strong weight for food distance
        int newDistanceToFood = getManhattanDistance(Position(nextPos), Position(food));
        // score += (newDistanceToFood - currentDistanceToFood) * 100;
        score += (newDistanceToFood - currentDistanceToFood) * 50;


        // Moderate weight for available space
        int spaceScore = countAccessibleSpace(Position(nextPos));
        score -= (GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT - spaceScore) * 10;
        
        // Small penalty for being near walls
        if (nextPos.x == 0 || nextPos.x == GameConfig::GRID_WIDTH - 1 ||
            nextPos.y == 0 || nextPos.y == GameConfig::GRID_HEIGHT - 1) {
            score += 50;
        }
        
        possibleMoves.push_back({dir, score});
    }
    
    if (!possibleMoves.empty()) {
        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });
        return possibleMoves[0].first;
    }
    
    return snake.getCurrentDirection();
}

std::vector<Direction> AIPlayer::findPathToFood() {
    std::set<Node> openSet;
    std::map<Position, Position> cameFrom;
    std::map<Position, int> gScore;
    
    Position start(snake.getHead());
    Node startNode(start, 0, getManhattanDistance(start, Position(food)));
    openSet.insert(startNode);
    gScore[start] = 0;
    
    while (!openSet.empty()) {
        Node current = *openSet.begin();
        openSet.erase(openSet.begin());
        
        if (current.pos == Position(food)) {
            // Reconstruct path
            std::vector<Direction> path;
            Position current(food);
            while (cameFrom.find(current) != cameFrom.end()) {
                Position prev = cameFrom[current];
                path.push_back(getDirectionToNeighbor(prev, current));
                current = prev;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        for (const auto& neighbor : getNeighbors(current.pos)) {
            // Skip if not safe
            Direction dir = getDirectionToNeighbor(current.pos, neighbor);
            if (!isMoveSafeInFuture(dir)) continue;  // Now uses default argument
            
            int tentativeG = gScore[current.pos] + 1;
            
            if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor]) {
                cameFrom[neighbor] = current.pos;
                gScore[neighbor] = tentativeG;
                int h = getManhattanDistance(neighbor, Position(food));
                Node neighborNode{neighbor, tentativeG, h};
                neighborNode.f = neighborNode.g + neighborNode.h;
                neighborNode.dirFromParent = dir;
                openSet.insert(neighborNode);
            }
        }
    }
    
    return std::vector<Direction>();  // No path found
}

Direction AIPlayer::calculateRandomMove() {
    std::vector<Direction> safeMoves;
    
    // Collect all safe moves
    if (isMoveSafe(Direction::Up)) safeMoves.push_back(Direction::Up);
    if (isMoveSafe(Direction::Down)) safeMoves.push_back(Direction::Down);
    if (isMoveSafe(Direction::Left)) safeMoves.push_back(Direction::Left);
    if (isMoveSafe(Direction::Right)) safeMoves.push_back(Direction::Right);
    
    if (!safeMoves.empty()) {
        // Pick a random safe move
        return safeMoves[rand() % safeMoves.size()];
    }
    
    return snake.getCurrentDirection();
}

Direction AIPlayer::calculateBasicMove() {
    const sf::Vector2i& head = snake.getHead();
    std::vector<std::pair<Direction, int>> possibleMoves;
    
    int currentDistance = std::abs(food.x - head.x) + std::abs(food.y - head.y);
    
    // Check each direction and score it
    auto checkMove = [&](Direction dir) {
        if (!isMoveSafe(dir)) return;
        
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

bool AIPlayer::isMoveSafe(Direction dir) {
    sf::Vector2i nextPos = snake.getHead();
    switch (dir) {
        case Direction::Up:    nextPos.y--; break;
        case Direction::Down:  nextPos.y++; break;
        case Direction::Left:  nextPos.x--; break;
        case Direction::Right: nextPos.x++; break;
    }
    
    // Check wall collision
    if (nextPos.x < 0 || nextPos.x >= GameConfig::GRID_WIDTH ||
        nextPos.y < 0 || nextPos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }
    
    // Check self collision
    const std::deque<sf::Vector2i>& snakeBody = snake.getBody();
    for (size_t i = 0; i < snakeBody.size(); ++i) {
        if (nextPos.x == snakeBody[i].x && nextPos.y == snakeBody[i].y) {
            return false;
        }
    }
    
    return true;
}

bool AIPlayer::isMoveSafe(Direction dir, int lookAhead) {
    if (lookAhead <= 0) return true;
    
    sf::Vector2i nextPos = snake.getHead();
    switch (dir) {
        case Direction::Up:    nextPos.y--; break;
        case Direction::Down:  nextPos.y++; break;
        case Direction::Left:  nextPos.x--; break;
        case Direction::Right: nextPos.x++; break;
    }
    
    // Basic collision checks
    if (nextPos.x < 0 || nextPos.x >= GameConfig::GRID_WIDTH ||
        nextPos.y < 0 || nextPos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }
    
    const auto& body = snake.getBody();
    for (const auto& segment : body) {
        if (nextPos == segment) return false;
    }
    
    // Look ahead recursively if needed
    if (lookAhead > 1) {
        bool foundSafeMove = false;
        // Check if at least one next move would be safe
        if (isMoveSafe(Direction::Up, lookAhead - 1)) foundSafeMove = true;
        if (isMoveSafe(Direction::Down, lookAhead - 1)) foundSafeMove = true;
        if (isMoveSafe(Direction::Left, lookAhead - 1)) foundSafeMove = true;
        if (isMoveSafe(Direction::Right, lookAhead - 1)) foundSafeMove = true;
        return foundSafeMove;
    }
    
    return true;
}

bool AIPlayer::isMoveSafeInFuture(Direction dir, int lookAhead) {
    if (lookAhead <= 0) return true;
    
    sf::Vector2i nextPos = snake.getHead();
    switch (dir) {
        case Direction::Up:    nextPos.y--; break;
        case Direction::Down:  nextPos.y++; break;
        case Direction::Left:  nextPos.x--; break;
        case Direction::Right: nextPos.x++; break;
    }
    
    // Check immediate collisions
    if (nextPos.x < 0 || nextPos.x >= GameConfig::GRID_WIDTH ||
        nextPos.y < 0 || nextPos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }
    
    // Check if move creates a tunnel with width 1 (dangerous!)
    bool leftBlocked = (nextPos.x == 0) || isPositionBlocked(Position(nextPos.x - 1, nextPos.y));
    bool rightBlocked = (nextPos.x == GameConfig::GRID_WIDTH - 1) || isPositionBlocked(Position(nextPos.x + 1, nextPos.y));
    bool topBlocked = (nextPos.y == 0) || isPositionBlocked(Position(nextPos.x, nextPos.y - 1));
    bool bottomBlocked = (nextPos.y == GameConfig::GRID_HEIGHT - 1) || isPositionBlocked(Position(nextPos.x, nextPos.y + 1));
    
    // If we're creating a tunnel, make sure our tail will clear it before we get trapped
    if ((leftBlocked && rightBlocked && !topBlocked && !bottomBlocked) ||
        (topBlocked && bottomBlocked && !leftBlocked && !rightBlocked)) {
        // Check if tail will clear this area before we potentially get trapped
        int distanceToTail = getDistanceToTail(nextPos);
        if (distanceToTail > snake.getBody().size() / 2) {
            return false;  // Too risky, tail is too far away
        }
    }
    
    // Check if we'll hit snake body that won't move in time
    const auto& body = snake.getBody();
    for (size_t i = 0; i < body.size(); ++i) {
        if (nextPos == body[i] && !willTailMove(body[i], i)) {
            return false;
        }
    }
    
    return true;
}

bool AIPlayer::willTailMove(const sf::Vector2i& pos, int segmentIndex) {
    // If this is the tail and we're not growing, it will move
    return segmentIndex == snake.getBody().size() - 1;
}

// Add implementation for countAccessibleSpace
int AIPlayer::countAccessibleSpace(const Position& from) {
    std::vector<std::vector<bool>> visited(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
    
    int space = 0;
    std::queue<Position> toVisit;
    toVisit.push(from);
    
    while (!toVisit.empty()) {
        Position pos = toVisit.front();
        toVisit.pop();
        
        if (pos.pos.x < 0 || pos.pos.x >= GameConfig::GRID_WIDTH || 
            pos.pos.y < 0 || pos.pos.y >= GameConfig::GRID_HEIGHT ||
            visited[pos.pos.x][pos.pos.y]) {
            continue;
        }
        
        visited[pos.pos.x][pos.pos.y] = true;
        space++;
        
        // Check if this position contains snake body
        bool isSnakeBody = false;
        const auto& snakeBody = snake.getBody();
        for (const auto& segment : snakeBody) {
            if (pos.pos.x == segment.x && pos.pos.y == segment.y) {
                isSnakeBody = true;
                break;
            }
        }
        if (isSnakeBody) continue;
        
        // Add adjacent cells
        toVisit.push(Position(pos.pos.x + 1, pos.pos.y));
        toVisit.push(Position(pos.pos.x - 1, pos.pos.y));
        toVisit.push(Position(pos.pos.x, pos.pos.y + 1));
        toVisit.push(Position(pos.pos.x, pos.pos.y - 1));
    }
    
    return space;
}

// Add implementation for canReachFood
bool AIPlayer::canReachFood(const Position& from) {
    return this->countAccessibleSpace(from) > 0;
}

std::vector<Position> AIPlayer::getNeighbors(const Position& pos) {
    std::vector<Position> neighbors;
    // Check all four directions
    neighbors.push_back(Position(pos.pos.x + 1, pos.pos.y));  // Right
    neighbors.push_back(Position(pos.pos.x - 1, pos.pos.y));  // Left
    neighbors.push_back(Position(pos.pos.x, pos.pos.y + 1));  // Down
    neighbors.push_back(Position(pos.pos.x, pos.pos.y - 1));  // Up
    
    // Filter out invalid positions
    neighbors.erase(
        std::remove_if(neighbors.begin(), neighbors.end(),
            [](const Position& p) {
                return p.pos.x < 0 || p.pos.x >= GameConfig::GRID_WIDTH ||
                       p.pos.y < 0 || p.pos.y >= GameConfig::GRID_HEIGHT;
            }),
        neighbors.end()
    );
    
    return neighbors;
}

Direction AIPlayer::getDirectionToNeighbor(const Position& from, const Position& to) {
    // Calculate difference
    int dx = to.pos.x - from.pos.x;
    int dy = to.pos.y - from.pos.y;
    
    // Determine direction based on largest difference
    if (dx > 0) return Direction::Right;
    if (dx < 0) return Direction::Left;
    if (dy > 0) return Direction::Down;
    if (dy < 0) return Direction::Up;
    
    // Default case (should never happen with valid neighbors)
    return Direction::Right;
}

bool AIPlayer::isPositionBlocked(const Position& pos) {
    if (pos.pos.x < 0 || pos.pos.x >= GameConfig::GRID_WIDTH ||
        pos.pos.y < 0 || pos.pos.y >= GameConfig::GRID_HEIGHT) {
        return true;
    }
    
    const auto& body = snake.getBody();
    for (const auto& segment : body) {
        if (pos.pos.x == segment.x && pos.pos.y == segment.y) {
            return true;
        }
    }
    return false;
}

int AIPlayer::getDistanceToTail(const sf::Vector2i& pos) {
    const auto& tail = snake.getBody().back();
    return std::abs(pos.x - tail.x) + std::abs(pos.y - tail.y);
}