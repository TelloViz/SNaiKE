#include "AI/ManhattanStrategy.hpp"

Direction ManhattanStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    // Calculate distances once and store them
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

        // Use the cached heat values instead of recalculating
        float score = heatValues[Position{nextPos}];
        if (score < bestScore) {
            bestScore = score;
            bestMove = dir;
        }
    }

    return bestMove;
}

void ManhattanStrategy::render(sf::RenderWindow& window) const {
    if (globalShowHeatMap) {  // Use the global state
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
}

void ManhattanStrategy::updateHeatMap(const Snake& snake, const sf::Vector2i& food) const {
    heatValues.clear();
    Position head{snake.getHead()};
    const auto& body = snake.getBody();
    
    for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
        for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
            Position pos{{x, y}};
            
            // Calculate base score from food and head distance
            float score = std::abs(pos.pos.x - food.x) + std::abs(pos.pos.y - food.y) +  // distance to food
                         (std::abs(pos.pos.x - head.pos.x) + std::abs(pos.pos.y - head.pos.y)) * 0.5f;  // distance to head
            
            // Check distance to snake body segments
            for (const auto& segment : body) {
                int distToSegment = std::abs(pos.pos.x - segment.x) + std::abs(pos.pos.y - segment.y);
                if (distToSegment < 2) {
                    score += 10.0f;
                    break;  // No need to check other segments once we found a close one
                }
            }
            
            heatValues[pos] = score;
        }
    }
}

void ManhattanStrategy::toggleHeatMap() {
    globalShowHeatMap = !globalShowHeatMap;
    std::cout << "Heat map: " << (globalShowHeatMap ? "ON" : "OFF") << std::endl;
}