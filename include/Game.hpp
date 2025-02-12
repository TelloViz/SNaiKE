#pragma once
#include <SFML/Graphics.hpp>
#include "Snake.hpp"

class Game {
private:
    static const int CELL_SIZE = 20;
    static const int GRID_WIDTH = 40;
    static const int GRID_HEIGHT = 30;

    sf::RenderWindow window;
    Snake snake;
    sf::Vector2i food;
    bool gameOver;

    void processEvents();
    void update();
    void render();
    void spawnFood();

public:
    Game();
    void run();
};