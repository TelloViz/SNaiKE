#include "AI/ManhattanStrategy.hpp"

Direction ManhattanStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    // Update heat map first
    updateHeatMap(snake, food);

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

void ManhattanStrategy::render(sf::RenderWindow& window) const {
    sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE - 2));
    
    for (const auto& [pos, value] : heatValues) {
        float normalizedValue = std::min(1.0f, value / 20.0f);
        
        sf::Color cellColor(
            static_cast<sf::Uint8>(255 * normalizedValue),
            static_cast<sf::Uint8>(255 * (1-normalizedValue)),
            0,
            100
        );
        
        cell.setFillColor(cellColor);
        cell.setPosition(
            pos.pos.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + 1,
            pos.pos.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + 1
        );
        window.draw(cell);
    }
}

void ManhattanStrategy::updateHeatMap(const Snake& snake, const sf::Vector2i& food) const {
    heatValues.clear();
    Position head{snake.getHead()};
    
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            Position pos{{x, y}};
            float distanceToFood = getManhattanDistance(pos, food);
            float distanceToHead = getManhattanDistance(pos, head.pos);
            
            heatValues[pos] = distanceToFood + distanceToHead * 0.5f;
            
            for (const auto& segment : snake.getBody()) {
                if (getManhattanDistance(pos.pos, segment) < 2) {
                    heatValues[pos] += 10.0f;
                }
            }
        }
    }
}