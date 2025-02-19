#include "Snake.hpp"

Snake::Snake() 
    : currentDirection(Direction::Right)
    , nextDirection(Direction::Right)
    , hasEaten(false)
    , isGrowing(false) {
    body.push_front(sf::Vector2i(5, 5));
    body.push_front(sf::Vector2i(6, 5));
    body.push_front(sf::Vector2i(7, 5));
}

Snake::Snake(int x, int y) 
    : currentDirection(Direction::Right)
    , nextDirection(Direction::Right)
    , hasEaten(false)
    , isGrowing(false) {
    body.push_front(sf::Vector2i(x-2, y));
    body.push_front(sf::Vector2i(x-1, y));
    body.push_front(sf::Vector2i(x, y));
}

Snake::Snake(const sf::Vector2i& startPos)
    : currentDirection(Direction::Right)
    , nextDirection(Direction::Right)
    , hasEaten(false)
    , isGrowing(false) {
    body.push_front(sf::Vector2i(startPos.x-2, startPos.y));
    body.push_front(sf::Vector2i(startPos.x-1, startPos.y));
    body.push_front(sf::Vector2i(startPos.x, startPos.y));
}

void Snake::setDirection(Direction newDir) {
    // Prevent 180-degree turns
    bool isValidMove = false;
    
    switch (currentDirection) {
        case Direction::Up:
            isValidMove = (newDir != Direction::Down);
            break;
        case Direction::Down:
            isValidMove = (newDir != Direction::Up);
            break;
        case Direction::Left:
            isValidMove = (newDir != Direction::Right);
            break;
        case Direction::Right:
            isValidMove = (newDir != Direction::Left);
            break;
    }
    
    if (isValidMove) {
        nextDirection = newDir;
    }
}

void Snake::move() {
    // Update current direction before moving
    updateDirection();
    
    // Get the current head position
    sf::Vector2i head = body.front();
    
    // Move based on current direction
    switch (currentDirection) {
        case Direction::Up:    head.y--; break;
        case Direction::Down:  head.y++; break;
        case Direction::Left:  head.x--; break;
        case Direction::Right: head.x++; break;
    }
    
    // Add new head and remove tail (unless growing)
    body.push_front(head);
    if (!isGrowing) {
        body.pop_back();
    } else {
        isGrowing = false;
    }
}

bool Snake::checkCollision(int gridWidth, int gridHeight) const {
    const auto& head = body.front();
    
    // Check wall collision
    if (head.x < 0 || head.x >= gridWidth ||
        head.y < 0 || head.y >= gridHeight) {
        return true;
    }
    
    return checkSelfCollision();
}

bool Snake::checkSelfCollision() const {
    if (body.size() < 2) return false;
    
    const auto& head = body.front();
    
    // Check self collision (excluding head)
    for (auto it = std::next(body.begin()); it != body.end(); ++it) {
        if (head.x == it->x && head.y == it->y) {
            return true;
        }
    }
    
    return false;
}

bool Snake::eat(const sf::Vector2i& food) {
    if (body.front() == food) {
        hasEaten = true;
        return true;
    }
    return false;
}

void Snake::draw(sf::RenderWindow& window, const int cellSize) {
    // Create much smaller segments with larger gaps
    float segmentSize = cellSize * 0.6f;  // Make segments 60% of cell size
    float offset = (cellSize - segmentSize) / 2;  // Center in cell
    
    sf::RectangleShape segment(sf::Vector2f(segmentSize, segmentSize));
    
    // Draw body segments
    segment.setFillColor(sf::Color::Green);
    auto it = std::next(body.begin());
    for (; it != body.end(); ++it) {
        segment.setPosition(
            it->x * cellSize + offset + GameConfig::MARGIN_SIDES,
            it->y * cellSize + offset + GameConfig::MARGIN_TOP
        );
        window.draw(segment);
    }
    
    // Draw head slightly larger
    float headSize = segmentSize * 1.2f;
    float headOffset = (cellSize - headSize) / 2;
    sf::RectangleShape head(sf::Vector2f(headSize, headSize));
    head.setFillColor(sf::Color::Yellow);
    head.setPosition(
        body.front().x * cellSize + headOffset + GameConfig::MARGIN_SIDES,
        body.front().y * cellSize + headOffset + GameConfig::MARGIN_TOP
    );
    window.draw(head);
}

void Snake::grow() {
    isGrowing = true;
}