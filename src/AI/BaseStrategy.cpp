#include "AI/BaseStrategy.hpp"
#include "GameConfig.hpp"
#include <queue>
#include <algorithm>
#include <array>
#include <iostream>

bool BaseStrategy::isMoveSafe(Direction dir, const Snake& snake) const {
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
    const auto& snakeBody = snake.getBody();
    return std::none_of(snakeBody.begin(), snakeBody.end(),
        [&nextPos](const auto& segment) { return nextPos == segment; });
}

int BaseStrategy::getManhattanDistance(const Position& a, const Position& b) const {
    return std::abs(a.pos.x - b.pos.x) + std::abs(a.pos.y - b.pos.y);
}

bool BaseStrategy::isPositionBlocked(const Position& pos, const Snake& snake) const {
    if (pos.pos.x < 0 || pos.pos.x >= GameConfig::GRID_WIDTH ||
        pos.pos.y < 0 || pos.pos.y >= GameConfig::GRID_HEIGHT) {
        return true;
    }
    
    const auto& body = snake.getBody();
    return std::any_of(body.begin(), body.end(),
        [&pos](const auto& segment) { 
            return pos.pos.x == segment.x && pos.pos.y == segment.y; 
        });
}

std::vector<Position> BaseStrategy::getNeighbors(const Position& pos) const {
    std::vector<Position> neighbors;
    neighbors.reserve(4);
    
    const std::array<std::pair<int, int>, 4> dirs = {
        {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
    };
    
    for (const auto& [dx, dy] : dirs) {
        Position newPos(pos.pos.x + dx, pos.pos.y + dy);
        if (newPos.pos.x >= 0 && newPos.pos.x < GameConfig::GRID_WIDTH &&
            newPos.pos.y >= 0 && newPos.pos.y < GameConfig::GRID_HEIGHT) {
            neighbors.push_back(newPos);
        }
    }
    
    return neighbors;
}

Direction BaseStrategy::getDirectionToNeighbor(const Position& from, const Position& to) const {
    int dx = to.pos.x - from.pos.x;
    int dy = to.pos.y - from.pos.y;
    
    if (dx > 0) return Direction::Right;
    if (dx < 0) return Direction::Left;
    if (dy > 0) return Direction::Down;
    if (dy < 0) return Direction::Up;
    
    return Direction::Right; // Default case
}

int BaseStrategy::countAccessibleSpace(const Position& from, const Snake& snake) const {
    std::cout << "Counting accessible space from: " << from.pos.x << "," << from.pos.y << std::endl;
    
    std::vector<std::vector<bool>> visited(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
    
    int space = 0;
    std::queue<Position> toVisit;
    toVisit.push(from);
    
    // Add maximum iterations to prevent infinite loops
    int maxIterations = GameConfig::GRID_WIDTH * GameConfig::GRID_HEIGHT;
    int iterations = 0;
    
    while (!toVisit.empty() && space < 64 && iterations < maxIterations) {
        iterations++;
        Position pos = toVisit.front();
        toVisit.pop();
        
        if (isPositionBlocked(pos, snake) || visited[pos.pos.x][pos.pos.y]) {
            continue;
        }
        
        visited[pos.pos.x][pos.pos.y] = true;
        space++;
        
        for (const auto& neighbor : getNeighbors(pos)) {
            if (!visited[neighbor.pos.x][neighbor.pos.y]) {
                toVisit.push(neighbor);
            }
        }
    }
    
    std::cout << "Space counting completed after " << iterations << " iterations" << std::endl;
    return space;
}

Direction BaseStrategy::calculateRandomMove(const Snake& snake) const {
    std::vector<Direction> safeMoves;
    
    // Collect all safe moves
    if (isMoveSafe(Direction::Up, snake)) safeMoves.push_back(Direction::Up);
    if (isMoveSafe(Direction::Down, snake)) safeMoves.push_back(Direction::Down);
    if (isMoveSafe(Direction::Left, snake)) safeMoves.push_back(Direction::Left);
    if (isMoveSafe(Direction::Right, snake)) safeMoves.push_back(Direction::Right);
    
    if (!safeMoves.empty()) {
        // Pick a random safe move
        return safeMoves[rand() % safeMoves.size()];
    }
    
    return snake.getCurrentDirection();
}