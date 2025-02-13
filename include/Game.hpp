#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Snake.hpp"
#include "State.hpp"
#include "states/States.hpp" 

class Game {
private:
    static const int CELL_SIZE = 20;
    static const int GRID_WIDTH = 40;
    static const int GRID_HEIGHT = 30;

    sf::RenderWindow window;
    std::unique_ptr<State> currentState;
    sf::Font font;

public:
    Game();
    void run();
    void processEvents();
    void update();
    void render();
    void changeState(std::unique_ptr<State> newState);
    const sf::Font& getFont() const { return font; }
    void quit() { window.close(); }

    static int getCellSize() { return CELL_SIZE; }
    static int getGridWidth() { return GRID_WIDTH; }
    static int getGridHeight() { return GRID_HEIGHT; }
};