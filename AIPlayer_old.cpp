#include "AI/AIPlayer.hpp"
#include "GameConfig.hpp"
#include <iostream>
#include "Debug.hpp"
#include <algorithm>
#include <limits>
#include <queue>
#include <set>
#include <map>

using std::min;
using std::max;

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
        AI_DEBUG("AI planning move: " << static_cast<int>(button));
        plannedMoves.push(GameInput{InputType::ButtonPressed, button});
    }
}

Direction AIPlayer::calculateDirectionToFood() {
    switch (currentStrategy) {
        case AIStrategy::Basic:
            return calculateBasicMove();
        case AIStrategy::Advanced:
            return calculateAdvancedMove();
        case AIStrategy::Random:
            return calculateRandomMove();
        default:
            return calculateBasicMove();
    }
}

Direction AIPlayer::calculateAdvancedMove() {
    static sf::Clock updateClock;
    static sf::Clock stuckClock;
    static sf::Vector2i lastPosition;
    static int stuckCount = 0;
    
    // Detect if snake is stuck
    if (snake.getHead() == lastPosition) {
        stuckCount++;
        if (stuckCount > 3) { // If stuck for more than 3 moves
            stuckCount = 0;
            return calculateRandomMove(); // Try a random move to break out
        }
    } else {
        stuckCount = 0;
        lastPosition = snake.getHead();
    }
    
    if (updateClock.getElapsedTime().asMilliseconds() > 200) {
        updateAdvancedHeatMap();
        updateClock.restart();
    }

    // Try to find a path to food first
    auto path = findPathToFood();
    if (!path.empty()) {
        Position nextPos(snake.getHead());
        Direction firstMove = path[0];
        
        // Verify next position safety
        switch (firstMove) {
            case Direction::Up:    nextPos.pos.y--; break;
            case Direction::Down:  nextPos.pos.y++; break;
            case Direction::Left:  nextPos.pos.x--; break;
            case Direction::Right: nextPos.pos.x++; break;
        }
        
        // Check if move is actually safe
        int space = countAccessibleSpace(nextPos);
        if (space >= snake.getBody().size()) {
            return firstMove;
        }
    }

    // If no safe path to food, use improved fallback logic
    std::vector<std::pair<Direction, float>> possibleMoves;
    const sf::Vector2i& head = snake.getHead();
    
    for (Direction dir : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        if (!isMoveSafeInFuture(dir, 5)) continue;
        
        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }
        
        float moveScore = calculatePositionScore(nextPos.x, nextPos.y);
        
        // Add stronger bias towards food direction
        int distanceToFood = std::abs(nextPos.x - food.x) + std::abs(nextPos.y - food.y);
        moveScore += (100.0f / (distanceToFood + 1));
        
        // Add bonus for moves that maintain distance from walls
        int wallDist = std::min({nextPos.x, nextPos.y, 
            GameConfig::GRID_WIDTH - 1 - nextPos.x, 
            GameConfig::GRID_HEIGHT - 1 - nextPos.y});
        moveScore += wallDist * 5.0f;
        
        // Reduced weight for continuing in same direction
        if (dir == snake.getCurrentDirection()) {
            moveScore += 2.0f;
        }
        
        possibleMoves.push_back({dir, moveScore});
    }
    
    if (!possibleMoves.empty()) {
        std::sort(possibleMoves.begin(), possibleMoves.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });
        return possibleMoves[0].first;
    }
    
    return calculateRandomMove(); // Last resort
}

std::vector<Direction> AIPlayer::findPathToFood() {
    static sf::Clock pathfindClock;
    static Position lastFoodPos;
    
    // Cache check
    if (lastFoodPos == Position(food) && 
        !lastPath.empty() && 
        pathfindClock.getElapsedTime().asMilliseconds() < 100) {
        Position current(snake.getHead());
        for (const Direction& dir : lastPath) {
            switch (dir) {
                case Direction::Up:    current.pos.y--; break;
                case Direction::Down:  current.pos.y++; break;
                case Direction::Left:  current.pos.x--; break;
                case Direction::Right: current.pos.x++; break;
            }
            if (isPositionBlocked(current)) {
                break;
            }
        }
        if (current == Position(food)) {
            return lastPath;
        }
    }
    
    lastFoodPos = Position(food);
    pathfindClock.restart();

    // A* implementation
    std::set<Node> openSet;
    std::map<Position, Position> cameFrom;
    std::map<Position, int> gScore;
    std::map<Position, Direction> directionToParent;
    
    Position start(snake.getHead());
    Position goal(food);

    Node startNode(start, 0, calculateHeuristic(start));
    openSet.insert(startNode);
    gScore[start] = 0;

    while (!openSet.empty()) {
        // Get node with lowest f score
        Node current = *openSet.begin();
        openSet.erase(openSet.begin());

        // Check if we reached the goal
        if (current.pos == goal) {
            // Reconstruct path
            std::vector<Direction> path;
            Position currentPos = current.pos;
            while (currentPos != start) {
                path.push_back(directionToParent[currentPos]);
                currentPos = cameFrom[currentPos];
            }
            std::reverse(path.begin(), path.end());
            lastPath = path;
            return path;
        }

        // Check neighbors
        for (const Position& neighbor : getNeighbors(current.pos)) {
            if (isPositionBlocked(neighbor)) continue;

            int tentativeG = gScore[current.pos] + 1;
            
            if (gScore.find(neighbor) == gScore.end() || 
                tentativeG < gScore[neighbor]) {
                // This path is better
                Direction dir = getDirectionToNeighbor(current.pos, neighbor);
                cameFrom[neighbor] = current.pos;
                directionToParent[neighbor] = dir;
                gScore[neighbor] = tentativeG;
                
                Node neighborNode(neighbor, tentativeG, calculateHeuristic(neighbor));
                openSet.insert(neighborNode);
            }
        }
    }

    // No path found
    lastPath.clear();
    return std::vector<Direction>();
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
    updateBasicHeatMap();
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

    // More aggressive tunnel detection
    if ((leftBlocked && rightBlocked) || (topBlocked && bottomBlocked)) {
        int distanceToTail = getDistanceToTail(nextPos);
        // More conservative threshold for tunnel entry
        if (distanceToTail > snake.getBody().size() / 3) {
            return false;  // Don't enter tunnels unless tail is very close
        }
    }

    // Avoid moves that lead to confined spaces
    int availableSpace = countAccessibleSpace(Position(nextPos));
    if (availableSpace < snake.getBody().size()) {
        return false;  // Don't move into spaces smaller than snake length
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
int AIPlayer::countAccessibleSpace(const Position& from) const {  // Make const
    static std::vector<std::vector<bool>> visited;
    visited.assign(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
    
    int space = 0;
    std::queue<Position> toVisit;
    toVisit.push(from);
    
    // Pre-calculate snake positions
    const auto& snakeBody = snake.getBody();
    std::vector<std::vector<bool>> snakePositions(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
    for (const auto& segment : snakeBody) {
        if (segment.x >= 0 && segment.x < GameConfig::GRID_WIDTH &&
            segment.y >= 0 && segment.y < GameConfig::GRID_HEIGHT) {
            snakePositions[segment.x][segment.y] = true;
        }
    }
    
    while (!toVisit.empty() && space < 64) { // Limit max space check
        Position pos = toVisit.front();
        toVisit.pop();
        
        if (pos.pos.x < 0 || pos.pos.x >= GameConfig::GRID_WIDTH || 
            pos.pos.y < 0 || pos.pos.y >= GameConfig::GRID_HEIGHT ||
            visited[pos.pos.x][pos.pos.y] ||
            snakePositions[pos.pos.x][pos.pos.y]) {
            continue;
        }
        
        visited[pos.pos.x][pos.pos.y] = true;
        space++;
        
        // Add adjacent cells
        if (space < 64) { // Only add more if under limit
            toVisit.push(Position(pos.pos.x + 1, pos.pos.y));
            toVisit.push(Position(pos.pos.x - 1, pos.pos.y));
            toVisit.push(Position(pos.pos.x, pos.pos.y + 1));
            toVisit.push(Position(pos.pos.x, pos.pos.y - 1));
        }
    }
    
    return space;
}

// Add implementation for canReachFood
bool AIPlayer::canReachFood(const Position& from) {
    return this->countAccessibleSpace(from) > 0;
}

std::vector<Position> AIPlayer::getNeighbors(const Position& pos) const {  // Add const
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

bool AIPlayer::isPositionBlocked(const Position& pos) const {  // Add const
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

float AIPlayer::calculatePositionScore(int x, int y) const {
    Position pos(x, y);
    float score = 0.0f;
    
    // Stronger weight for food distance
    score -= getManhattanDistance(pos, Position(food)) * 8.0f;
    
    // Increased penalty for wall proximity
    int wallDist = std::min({x, y, GameConfig::GRID_WIDTH - 1 - x, 
                           GameConfig::GRID_HEIGHT - 1 - y});
    score += wallDist * 3.0f;
    
    // More weight on available space
    int spaceScore = countAccessibleSpace(pos);
    score += spaceScore * 1.0f;
    
    // Reduce head distance influence
    int distanceToHead = getManhattanDistance(pos, Position(snake.getHead()));
    if (distanceToHead > 0) {
        score -= distanceToHead * 0.5f;
    }
    
    // Stronger wall penalties
    if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
        y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
        score -= 20.0f;
    }

    // Additional penalty for corner positions
    if ((x == 0 || x == GameConfig::GRID_WIDTH - 1) &&
        (y == 0 || y == GameConfig::GRID_HEIGHT - 1)) {
        score -= 30.0f;
    }
    
    return score;
}

int AIPlayer::getManhattanDistance(const Position& a, const Position& b) const {    return std::abs(a.pos.x - b.pos.x) + std::abs(a.pos.y - b.pos.y);
}

void AIPlayer::updateHeatMap() {
    static sf::Clock updateClock;
    if (updateClock.getElapsedTime().asMilliseconds() < 100) {
        return;
    }
    updateClock.restart();
    
    heatMap.clear();
    const sf::Vector2i& snakeHead = snake.getHead();
    const auto& snakeBody = snake.getBody();
    
    // Pre-calculate snake body positions for faster lookup
    std::vector<std::vector<bool>> snakePositions(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
        
    for (const auto& segment : snakeBody) {
        if (segment.x >= 0 && segment.x < GameConfig::GRID_WIDTH &&
            segment.y >= 0 && segment.y < GameConfig::GRID_HEIGHT) {
            snakePositions[segment.x][segment.y] = true;
        }
    }
    
    // Only calculate for positions near the snake and food
    const int VIEW_RADIUS = 8;
    int minX = std::max(0, std::min(snakeHead.x, food.x) - VIEW_RADIUS);
    int maxX = std::min(GameConfig::GRID_WIDTH, std::max(snakeHead.x, food.x) + VIEW_RADIUS);
    int minY = std::max(0, std::min(snakeHead.y, food.y) - VIEW_RADIUS);
    int maxY = std::min(GameConfig::GRID_HEIGHT, std::max(snakeHead.y, food.y) + VIEW_RADIUS);
    
    for (int x = minX; x < maxX; x++) {
        for (int y = minY; y < maxY; y++) {
            // Skip if this is a snake body position
            if (snakePositions[x][y]) {
                heatMap.setValue(x, y, -150);
                continue;
            }
            
            Position pos(x, y);
            float score = 0.1f;
            
            // Simplified scoring
            score -= getManhattanDistance(pos, Position(food)) * 10;
            score -= getManhattanDistance(pos, Position(snakeHead)) * 5;  // Use snakeHead instead of head
            
            // Quick wall distance check
            int wallDist = std::min({x, y, 
                GameConfig::GRID_WIDTH - 1 - x, 
                GameConfig::GRID_HEIGHT - 1 - y});
            score += wallDist * 5;
            
            // Add food bonus
            if (x == food.x && y == food.y) {
                score += 150;
            }
            
            // Simplified space check
            if (wallDist > 0 && !snakePositions[x][y]) {
                score += 20;
            }
            
            heatMap.setValue(x, y, score);
        }
    }
}

void AIPlayer::updateBasicHeatMap() {
    basicHeatMap.clear();
    
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            Position pos(x, y);
            float score = 0.1f;
            
            // Basic heat map scoring
            score -= getManhattanDistance(pos, Position(food)) * 15.0f;
            
            // Wall penalties
            if (x == 0 || x == GameConfig::GRID_WIDTH - 1 ||
                y == 0 || y == GameConfig::GRID_HEIGHT - 1) {
                score -= 50.0f;
            }
            
            basicHeatMap.setValue(x, y, score);
        }
    }
}

void AIPlayer::updateAdvancedHeatMap() {
    advancedHeatMap.clear();
    const sf::Vector2i& head = snake.getHead();
    const auto& body = snake.getBody();
    
    // Pre-calculate snake body positions
    std::vector<std::vector<bool>> snakePositions(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
        
    for (const auto& bodyPart : body) {
        if (bodyPart.x >= 0 && bodyPart.x < GameConfig::GRID_WIDTH &&
            bodyPart.y >= 0 && bodyPart.y < GameConfig::GRID_HEIGHT) {
            snakePositions[bodyPart.x][bodyPart.y] = true;
        }
    }
    
    // Only update positions near snake and food for performance
    const int VIEW_RADIUS = 8;
    int minX = std::max(0, std::min(head.x, food.x) - VIEW_RADIUS);
    int maxX = std::min(GameConfig::GRID_WIDTH - 1, std::max(head.x, food.x) + VIEW_RADIUS);
    int minY = std::max(0, std::min(head.y, food.y) - VIEW_RADIUS);
    int maxY = std::min(GameConfig::GRID_HEIGHT - 1, std::max(head.y, food.y) + VIEW_RADIUS);
    
    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            if (snakePositions[x][y]) {
                advancedHeatMap.setValue(x, y, -150);
                continue;
            }
            
            Position pos(x, y);
            float score = calculatePositionScore(x, y);
            advancedHeatMap.setValue(x, y, score);
        }
    }
}

int AIPlayer::calculateHeuristic(const Position& pos) const {
    // Base cost is distance to food
    int h = std::abs(pos.pos.x - food.x) + std::abs(pos.pos.y - food.y);
    
    // Add cost for confined spaces
    int space = countAccessibleSpace(pos);
    if (space < snake.getBody().size() * 1.5) {
        h += 50;  // Heavy penalty for too small spaces
    }
    
    // Add cost for positions that might trap the snake
    int exits = 0;
    for (const auto& neighbor : getNeighbors(pos)) {
        if (!isPositionBlocked(neighbor)) exits++;
    }
    if (exits <= 1) h += 100;  // Heavy penalty for potential traps
    
    return h;
}

