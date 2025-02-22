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
    
    return !isPositionBlocked(Position(nextPos), snake);
}

// Remove getManhattanDistance implementation from here since it's now in the header

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

int BaseStrategy::countAccessibleSpace(const Position& start, const Snake& snake) const {
    static const int MAX_ITERATIONS = 50;  // Limit iterations
    int count = 0;
    std::vector<std::vector<bool>> visited(GameConfig::GRID_WIDTH, 
        std::vector<bool>(GameConfig::GRID_HEIGHT, false));
    
    std::queue<Position> queue;
    queue.push(start);
    visited[start.pos.x][start.pos.y] = true;
    
    int iterations = 0;
    while (!queue.empty() && iterations < MAX_ITERATIONS) {
        Position current = queue.front();
        queue.pop();
        count++;
        iterations++;
        
        for (const auto& neighbor : getNeighbors(current)) {
            if (!visited[neighbor.pos.x][neighbor.pos.y] && !isPositionBlocked(neighbor, snake)) {
                queue.push(neighbor);
                visited[neighbor.pos.x][neighbor.pos.y] = true;
            }
        }
    }
    
    return count;
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