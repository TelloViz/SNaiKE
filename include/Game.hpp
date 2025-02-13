#pragma once
#include <SFML/Graphics.hpp>
#include "GameController.hpp"
#include "GameConfig.hpp"

class Game {
private:
    sf::RenderWindow window; // SFML window
    sf::Font font; // SFML font
    GameController gameController; // Controls Game Flow via State Machine

public:
    Game();
    void run();
    void processEvents();
    void update();
    void render();
};