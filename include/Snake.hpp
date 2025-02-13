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
    Snake();  // Keep default constructor
    Snake(int x, int y);  // Add position constructor
    Snake(const sf::Vector2i& startPos);  // Add Vector2i constructor
    
    void setDirection(Direction newDir);
    void move();
    void grow();
    bool checkCollision(const int gridWidth, const int gridHeight);
    bool eat(const sf::Vector2i& food);
    void draw(sf::RenderWindow& window, const int cellSize);
    const std::deque<sf::Vector2i>& getBody() const { return body; }
    const sf::Vector2i& getHead() const { return body.front(); }
};
