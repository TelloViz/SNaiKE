#pragma once
#include <SFML/Graphics.hpp>
#include <deque>

/**
 * @brief Represents possible movement directions for the snake
 */
enum class Direction { 
    Up,     ///< Moving upwards (-y)
    Down,   ///< Moving downwards (+y)
    Left,   ///< Moving left (-x)
    Right   ///< Moving right (+x)
};

/**
 * @brief Represents the snake entity in the game
 * 
 * The Snake class manages:
 * - Snake body segments as a deque of positions
 * - Movement and direction control
 * - Collision detection with walls and self
 * - Growth mechanics when eating food
 */
class Snake {
private:
    std::deque<sf::Vector2i> body;    ///< Snake body segments, front() is head
    Direction currentDirection;       ///< Current movement direction
    Direction nextDirection;          ///< Next movement direction
    bool hasEaten;                    ///< Flag for pending growth
    bool isGrowing;                   ///< Flag for ongoing growth

public:
    /**
     * @brief Default constructor, creates snake at (0,0)
     */
    Snake();

    /**
     * @brief Creates snake at specified grid coordinates
     * @param x X-coordinate in grid
     * @param y Y-coordinate in grid
     */
    Snake(int x, int y);

    /**
     * @brief Creates snake at specified position
     * @param startPos Starting position vector
     */
    Snake(const sf::Vector2i& startPos);

    /**
     * @brief Sets new movement direction
     * @param newDir New direction to move
     * @note Cannot reverse directly into opposite direction
     */
    void setDirection(Direction newDir);
    Direction getCurrentDirection() const { return currentDirection; }
    void updateDirection() { currentDirection = nextDirection; }

    /**
     * @brief Updates snake position based on current direction
     * Moves all body segments and handles growth if food was eaten
     */
    void move();

    /**
     * @brief Triggers snake growth on next move
     */
    void grow();

    /**
     * @brief Checks for collisions with walls or self
     * @param gridWidth Width of game grid
     * @param gridHeight Height of game grid
     * @return true if collision detected, false otherwise
     */
    bool checkCollision(int gridWidth, int gridHeight) const;

    /**
     * @brief Checks if snake head collides with food
     * @param food Position of food in grid
     * @return true if food eaten, false otherwise
     */
    bool eat(const sf::Vector2i& food);

    /**
     * @brief Renders snake to game window
     * @param window SFML window to render to
     * @param cellSize Size of each grid cell in pixels
     */
    void draw(sf::RenderWindow& window, const int cellSize);

    /**
     * @brief Gets const reference to snake body
     * @return Deque of body segment positions
     */
    const std::deque<sf::Vector2i>& getBody() const { return body; }

    /**
     * @brief Gets position of snake head
     * @return Vector representing head position
     */
    const sf::Vector2i& getHead() const { return body.front(); }

    /**
     * @brief Checks for self-collision
     * @return true if self-collision detected, false otherwise
     */
    bool checkSelfCollision() const;
};
