#include "AI/HamiltonStrategy.hpp"
#include <iostream>

HamiltonStrategy::HamiltonStrategy(const Snake& snakeRef)
    : snake(snakeRef)
    , currentIndex(0)
    , cycleInitialized(false)
{
    initializeHamiltonCycle();
    // Find the closest point in the cycle to the snake's starting position
    size_t startPos = findSnakeHeadInCycle();
    
    // Rotate the cycle so it starts at the snake's position
    std::rotate(hamiltonCycle.begin(), 
                hamiltonCycle.begin() + startPos,
                hamiltonCycle.end());
}

void HamiltonStrategy::initializeHamiltonCycle() {
    hamiltonCycle.clear();
    
    int width = GameConfig::GRID_WIDTH;
    int height = GameConfig::GRID_HEIGHT;

    // Start at top-left corner
    // Fill grid in a snake pattern, row by row
    for (int y = 0; y < height; y++) {
        if (y % 2 == 0) {
            // Even rows: go right
            for (int x = 0; x < width; x++) {
                hamiltonCycle.push_back({x, y});
            }
            // Connect to next row if not last row
            if (y < height - 1) {
                hamiltonCycle.push_back({width - 1, y + 1});
            }
        } else {
            // Odd rows: go left
            for (int x = width - 1; x >= 0; x--) {
                hamiltonCycle.push_back({x, y});
            }
            // Connect to next row if not last row
            if (y < height - 1) {
                hamiltonCycle.push_back({0, y + 1});
            }
        }
    }

    // Add return path to start
    // Go up the leftmost column if we ended on the left
    // Go up the rightmost column if we ended on the right
    bool endedOnLeft = (height % 2 == 1);
    int returnColumn = endedOnLeft ? 0 : width - 1;
    
    for (int y = height - 1; y > 0; y--) {
        hamiltonCycle.push_back({returnColumn, y});
    }

    cycleInitialized = true;
}

Direction HamiltonStrategy::calculateNextMove(const Snake& snake, const sf::Vector2i& food) {
    if (!cycleInitialized) return Direction::Right;

    // Find current position in cycle
    size_t currentPos = findSnakeHeadInCycle();
    
    // Get next position (wrap around if at end)
    size_t nextPos = (currentPos + 1) % hamiltonCycle.size();

    // Get current and next coordinates
    sf::Vector2i current = snake.getHead();
    sf::Vector2i next = hamiltonCycle[nextPos];

    // Debug output
    std::cout << "Current: " << current.x << "," << current.y 
              << " Next: " << next.x << "," << next.y << std::endl;

    // Calculate direction to next position
    if (next.x > current.x) return Direction::Right;
    if (next.x < current.x) return Direction::Left;
    if (next.y > current.y) return Direction::Down;
    return Direction::Up;
}

size_t HamiltonStrategy::findSnakeHeadInCycle() const {
    sf::Vector2i head = snake.getHead();
    for (size_t i = 0; i < hamiltonCycle.size(); i++) {
        if (hamiltonCycle[i] == head) {
            return i;
        }
    }
    return 0;  // Fallback
}

void HamiltonStrategy::update() {
    // No update needed for basic Hamilton
}

void HamiltonStrategy::render(sf::RenderWindow& window) const {
    if (globalShowHeatMap && cycleInitialized) {
        // Render the Hamilton cycle path
        sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE - 2));
        for (size_t i = 0; i < hamiltonCycle.size(); i++) {
            float heat = static_cast<float>(i) / hamiltonCycle.size();
            cell.setFillColor(sf::Color(
                static_cast<sf::Uint8>(255 * heat),
                0,
                static_cast<sf::Uint8>(255 * (1-heat)),
                96
            ));
            
            cell.setPosition(
                hamiltonCycle[i].x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + 1,
                hamiltonCycle[i].y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + 1
            );
            window.draw(cell);
        }
    }

    if (showPathArrows && cycleInitialized) {
        sf::ConvexShape arrow;
        arrow.setPointCount(3);
        arrow.setPoint(0, sf::Vector2f(0, -4));    // Top point
        arrow.setPoint(1, sf::Vector2f(-4, 4));    // Bottom left
        arrow.setPoint(2, sf::Vector2f(4, 4));     // Bottom right
        arrow.setFillColor(sf::Color(255, 255, 255, 180));

        sf::Vector2i pos = snake.getHead();
        size_t currentPos = findSnakeHeadInCycle();

        // Draw arrows for next few steps in cycle
        for (size_t i = 0; i < std::min(size_t(10), hamiltonCycle.size() - currentPos); i++) {
            size_t idx = (currentPos + i) % hamiltonCycle.size();
            sf::Vector2i next = hamiltonCycle[(idx + 1) % hamiltonCycle.size()];
            sf::Vector2i current = hamiltonCycle[idx];


            std::cout << "Current Head: (" << pos.x << ", " << pos.y 
            << ") -> Next Target: (" << next.x << ", " << next.y << ")\n";

            // Calculate rotation based on direction
            float rotation = 0.0f;
            if (next.x > current.x) rotation = 90.0f;
            else if (next.x < current.x) rotation = -90.0f;
            else if (next.y > current.y) rotation = 180.0f;

            arrow.setRotation(rotation);
            arrow.setPosition(
                current.x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES + GameConfig::CELL_SIZE / 2,
                current.y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP + GameConfig::CELL_SIZE / 2
            );
            window.draw(arrow);
        }
    }
}

void HamiltonStrategy::toggleHeatMap() {
    globalShowHeatMap = !globalShowHeatMap;
    std::cout << "Heat map: " << (globalShowHeatMap ? "ON" : "OFF") << std::endl;
}