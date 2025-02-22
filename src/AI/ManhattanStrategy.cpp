#include "AI/ManhattanStrategy.hpp"

Direction ManhattanStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    sf::Vector2i head = snake.getHead();
    std::vector<Direction> possibleMoves = {
        Direction::Up, Direction::Down, Direction::Left, Direction::Right
    };

    Direction bestMove = snake.getCurrentDirection();
    float bestScore = std::numeric_limits<float>::max();

    for (Direction dir : possibleMoves) {
        if (!isMoveSafe(dir, snake)) continue;

        sf::Vector2i nextPos = head;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }

        float score = getManhattanDistance(nextPos, food);
        if (score < bestScore) {
            bestScore = score;
            bestMove = dir;
        }
    }

    return bestMove;
}