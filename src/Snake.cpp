#include "Snake.hpp"

Snake::Snake() : direction(Direction::Right), hasEaten(false) {
    body.push_front(sf::Vector2i(5, 5));
    body.push_front(sf::Vector2i(6, 5));
    body.push_front(sf::Vector2i(7, 5));
}

Snake::Snake(int x, int y) 
    : direction(Direction::Right)
    , hasEaten(false) {
    // Initialize with 3 segments
    body.push_front(sf::Vector2i(x-2, y));
    body.push_front(sf::Vector2i(x-1, y));
    body.push_front(sf::Vector2i(x, y));
}

Snake::Snake(const sf::Vector2i& startPos)
    : direction(Direction::Right)
    , hasEaten(false) {
    // Initialize with 3 segments
    body.push_front(sf::Vector2i(startPos.x-2, startPos.y));
    body.push_front(sf::Vector2i(startPos.x-1, startPos.y));
    body.push_front(sf::Vector2i(startPos.x, startPos.y));
}

void Snake::setDirection(Direction newDir) {
    if ((direction == Direction::Up && newDir != Direction::Down) ||
        (direction == Direction::Down && newDir != Direction::Up) ||
        (direction == Direction::Left && newDir != Direction::Right) ||
        (direction == Direction::Right && newDir != Direction::Left)) {
        direction = newDir;
    }
}

void Snake::move() {
    // Create new head position based on current direction
    sf::Vector2i newHead = body.front();
    switch (direction) {
        case Direction::Up:    newHead.y--; break;
        case Direction::Down:  newHead.y++; break;
        case Direction::Left:  newHead.x--; break;
        case Direction::Right: newHead.x++; break;
    }
    
    // Add new head
    body.push_front(newHead);
    
    // Only remove tail if we haven't eaten
    if (!hasEaten) {
        body.pop_back();
    } else {
        hasEaten = false;  // Reset the flag
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
    for (const auto& segment : body) {
        sf::RectangleShape rect(sf::Vector2f(cellSize - 1, cellSize - 1));
        rect.setPosition(segment.x * cellSize, segment.y * cellSize);
        rect.setFillColor(sf::Color::Green);
        window.draw(rect);
    }
}

void Snake::grow() {
    // Set the flag that will be checked in the next move()
    hasEaten = true;
}