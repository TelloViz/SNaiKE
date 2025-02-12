#pragma once
#include <SFML/Graphics.hpp>
#include <deque>

enum class Direction { Up, Down, Left, Right };

class Snake {
private:
    std::deque<sf::Vector2i> body;
    Direction direction;
    bool hasEaten;

public:
    Snake();
    void setDirection(Direction newDir);
    void move();
    bool checkCollision(const int gridWidth, const int gridHeight);
    bool eat(const sf::Vector2i& food);
    void draw(sf::RenderWindow& window, const int cellSize);
};