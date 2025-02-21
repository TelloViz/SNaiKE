#include "AI/RandomStrategy.hpp"
#include <vector>
#include <cstdlib>

Direction RandomStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
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