#include "AI/FloodFillStrategy.hpp"
#include "AI/BaseStrategy.hpp"
#include "GameConfig.hpp"
#include <queue>
#include <vector>
#include <iostream>

Direction FloodFillStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    this->food = food;  // Store food position for arrow rendering
    
    std::vector<Direction> possibleMoves = {
        Direction::Up, Direction::Down, Direction::Left, Direction::Right
    };

    Direction bestMove = snake.getCurrentDirection();
    int maxSpace = -1;
    float bestScore = std::numeric_limits<float>::max();

    // Get minimum required space for snake to survive
    int minRequiredSpace = snake.getBody().size() + 1;

    for (Direction dir : possibleMoves) {
        sf::Vector2i nextPos = snake.getHead();
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }

        if (!isAreaLargeEnough(snake, nextPos)) continue;

        int space = floodFill(snake, nextPos);
        float foodDist = getManhattanDistance(nextPos, food);
        
        // New scoring system:
        // - If next position is food, give it high priority if there's enough space
        // - Otherwise, balance between space and food distance
        float score;
        if (nextPos == food && space >= minRequiredSpace) {
            score = -1000;  // Highly favorable score for food if safe
        } else {
            // Adjust weights: reduce the impact of space limitation
            score = (100.0f / (space + 1)) + (foodDist * 2.0f);
            
            // Add penalty for moving away from food
            if (getManhattanDistance(snake.getHead(), food) < foodDist) {
                score += 50.0f;
            }
        }
        
        if (score < bestScore) {
            bestScore = score;
            maxSpace = space;
            bestMove = dir;
        }
    }

    return bestMove;
}

int FloodFillStrategy::floodFill(const Snake& snake, const sf::Vector2i& startPos) const {
    if (!isAreaLargeEnough(snake, startPos)) return 0;

    std::vector<std::vector<bool>> visited(
        GameConfig::GRID_WIDTH,
        std::vector<bool>(GameConfig::GRID_HEIGHT, false)
    );

    std::queue<std::pair<sf::Vector2i, int>> queue;  // Position and distance
    queue.push({startPos, 0});
    visited[startPos.x][startPos.y] = true;
    int weightedAccessibility = 0;
    const int MAX_DISTANCE = 8;  // Only consider cells within this distance

    while (!queue.empty()) {
        auto [current, distance] = queue.front();
        queue.pop();

        if (distance > MAX_DISTANCE) continue;
        
        // Cells closer to the starting position contribute more to the score
        weightedAccessibility += (MAX_DISTANCE - distance + 1);

        // Check all adjacent cells
        std::vector<sf::Vector2i> neighbors = {
            {current.x, current.y - 1},  // Up
            {current.x, current.y + 1},  // Down
            {current.x - 1, current.y},  // Left
            {current.x + 1, current.y}   // Right
        };

        for (const auto& next : neighbors) {
            if (next.x < 0 || next.x >= GameConfig::GRID_WIDTH ||
                next.y < 0 || next.y >= GameConfig::GRID_HEIGHT ||
                visited[next.x][next.y]) {
                continue;
            }

            // Skip if contains snake body
            bool isBody = false;
            for (const auto& segment : snake.getBody()) {
                if (next == segment) {
                    isBody = true;
                    break;
                }
            }
            if (isBody) continue;

            visited[next.x][next.y] = true;
            queue.push({next, distance + 1});
        }
    }

    return weightedAccessibility;
}

bool FloodFillStrategy::isAreaLargeEnough(const Snake& snake, const sf::Vector2i& nextPos) const {
    // Check if the position is within bounds
    if (nextPos.x < 0 || nextPos.x >= GameConfig::GRID_WIDTH ||
        nextPos.y < 0 || nextPos.y >= GameConfig::GRID_HEIGHT) {
        return false;
    }

    // Check if position collides with snake body
    const auto& body = snake.getBody();
    for (const auto& segment : body) {
        if (nextPos == segment) return false;
    }

    // Minimum required space is snake length
    return true;
}

void FloodFillStrategy::render(sf::RenderWindow& window) const {
    if (globalShowHeatMap) {
        // Initialize or update heatmap every few frames
        static int updateCounter = 0;
        if (heatMap.empty() || updateCounter++ % 5 == 0) {
            heatMap.assign(GameConfig::GRID_WIDTH, 
                          std::vector<int>(GameConfig::GRID_HEIGHT, 0));
            
            // Calculate accessible space from each position
            int maxSpace = 1;
            for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
                for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
                    sf::Vector2i pos{x, y};
                    // Skip positions occupied by snake
                    bool isSnakeBody = false;
                    for (const auto& segment : snake.getBody()) {
                        if (segment == pos) {
                            isSnakeBody = true;
                            break;
                        }
                    }
                    if (isSnakeBody) {
                        heatMap[x][y] = 0;
                        continue;
                    }
                    
                    // Calculate accessible space
                    int space = floodFill(snake, pos);
                    heatMap[x][y] = space;
                    maxSpace = std::max(maxSpace, space);
                }
            }
            const_cast<FloodFillStrategy*>(this)->maxHeatValue = maxSpace;
        }

        // Render the heatmap
        sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE - 2, 
                                            GameConfig::CELL_SIZE - 2));

        for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
                float value = static_cast<float>(heatMap[x][y]) / maxHeatValue;
                
                cell.setPosition(
                    x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + 1,
                    y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + 1
                );

                // Simpler color scheme: Red (dangerous) to Blue (safe)
                cell.setFillColor(sf::Color(
                    static_cast<sf::Uint8>(255 * (1.0f - value)), // Red
                    0,                                            // Green
                    static_cast<sf::Uint8>(255 * value),         // Blue
                    128                                          // Alpha
                ));
                window.draw(cell);
            }
        }
    }

    // Render path arrows if enabled
    if (showPathArrows) {
        sf::Vector2i current = snake.getHead();
        renderArrows(window, current);
    }
}

void FloodFillStrategy::renderArrows(sf::RenderWindow& window, const sf::Vector2i& start) const {
    sf::ConvexShape arrow;
    arrow.setPointCount(3);
    arrow.setFillColor(sf::Color(0, 255, 0, 180));
    float arrowSize = GameConfig::CELL_SIZE * 0.4f;

    // Plan path to food using flood fill values
    std::vector<Direction> plannedPath;
    sf::Vector2i current = start;
    sf::Vector2i foodPos = food;  // We need to store food position as member

    // Plan up to 8 moves ahead
    for (int step = 0; step < 8; step++) {
        std::vector<Direction> possibleMoves = {
            Direction::Up, Direction::Down, Direction::Left, Direction::Right
        };

        Direction bestMove = snake.getCurrentDirection();
        int maxSpace = -1;
        float bestScore = std::numeric_limits<float>::max();

        for (Direction dir : possibleMoves) {
            sf::Vector2i nextPos = current;
            switch (dir) {
                case Direction::Up:    nextPos.y--; break;
                case Direction::Down:  nextPos.y++; break;
                case Direction::Left:  nextPos.x--; break;
                case Direction::Right: nextPos.x++; break;
            }

            if (!isAreaLargeEnough(snake, nextPos)) continue;

            int space = floodFill(snake, nextPos);
            float foodDist = getManhattanDistance(nextPos, foodPos);
            
            // Score combines available space and distance to food
            float score = (1000.0f / (space + 1)) + foodDist;
            
            if (score < bestScore) {
                bestScore = score;
                maxSpace = space;
                bestMove = dir;
            }
        }

        // If no valid moves found, stop planning
        if (maxSpace == -1) break;

        plannedPath.push_back(bestMove);
        
        // Update current position for next iteration
        switch (bestMove) {
            case Direction::Up:    current.y--; break;
            case Direction::Down:  current.y++; break;
            case Direction::Left:  current.x--; break;
            case Direction::Right: current.x++; break;
        }

        // Stop if we've reached the food
        if (current == foodPos) break;
    }

    // Render the planned path
    current = start;
    for (Direction dir : plannedPath) {
        switch (dir) {
            case Direction::Up:
                arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE/2, 2));
                arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE/2 - arrowSize, arrowSize + 2));
                arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE/2 + arrowSize, arrowSize + 2));
                current.y--;
                break;
            case Direction::Down:
                arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE/2, GameConfig::CELL_SIZE - 2));
                arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE/2 - arrowSize, GameConfig::CELL_SIZE - arrowSize - 2));
                arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE/2 + arrowSize, GameConfig::CELL_SIZE - arrowSize - 2));
                current.y++;
                break;
            case Direction::Left:
                arrow.setPoint(0, sf::Vector2f(2, GameConfig::CELL_SIZE/2));
                arrow.setPoint(1, sf::Vector2f(arrowSize + 2, GameConfig::CELL_SIZE/2 - arrowSize));
                arrow.setPoint(2, sf::Vector2f(arrowSize + 2, GameConfig::CELL_SIZE/2 + arrowSize));
                current.x--;
                break;
            case Direction::Right:
                arrow.setPoint(0, sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE/2));
                arrow.setPoint(1, sf::Vector2f(GameConfig::CELL_SIZE - arrowSize - 2, GameConfig::CELL_SIZE/2 - arrowSize));
                arrow.setPoint(2, sf::Vector2f(GameConfig::CELL_SIZE - arrowSize - 2, GameConfig::CELL_SIZE/2 + arrowSize));
                current.x++;
                break;
        }

        arrow.setPosition(
            (current.x * GameConfig::CELL_SIZE) + GameConfig::MARGIN_SIDES,
            (current.y * GameConfig::CELL_SIZE) + GameConfig::MARGIN_TOP
        );
        window.draw(arrow);
    }
}

Direction FloodFillStrategy::findBestMove(const sf::Vector2i& pos) const {
    std::vector<Direction> possibleMoves = {
        Direction::Up, Direction::Down, Direction::Left, Direction::Right
    };

    Direction bestMove = Direction::Right;  // Default
    int maxSpace = -1;

    for (Direction dir : possibleMoves) {
        sf::Vector2i nextPos = pos;
        switch (dir) {
            case Direction::Up:    nextPos.y--; break;
            case Direction::Down:  nextPos.y++; break;
            case Direction::Left:  nextPos.x--; break;
            case Direction::Right: nextPos.x++; break;
        }

        if (!isAreaLargeEnough(snake, nextPos)) continue;

        int space = floodFill(snake, nextPos);
        if (space > maxSpace) {
            maxSpace = space;
            bestMove = dir;
        }
    }

    return bestMove;
}