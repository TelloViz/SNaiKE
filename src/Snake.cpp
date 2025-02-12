#include "Snake.hpp"

Snake::Snake() : direction(Direction::Right), hasEaten(false) {
    body.push_front(sf::Vector2i(5, 5));
    body.push_front(sf::Vector2i(6, 5));
    body.push_front(sf::Vector2i(7, 5));
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
    sf::Vector2i head = body.front();
    switch (direction) {
        case Direction::Up: head.y--; break;
        case Direction::Down: head.y++; break;
        case Direction::Left: head.x--; break;
        case Direction::Right: head.x++; break;
    }
    body.push_front(head);
    if (!hasEaten) {
        body.pop_back();
    }
    hasEaten = false;
}

bool Snake::checkCollision(const int gridWidth, const int gridHeight) {
    sf::Vector2i head = body.front();
    
    if (head.x < 0 || head.x >= gridWidth || 
        head.y < 0 || head.y >= gridHeight) {
        return true;
    }

    for (size_t i = 1; i < body.size(); i++) {
        if (head == body[i]) return true;
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