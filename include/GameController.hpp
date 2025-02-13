#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include "Snake.hpp"

class GameController {
private:
    StateMachine stateMachine;
    Snake snake;
    sf::Vector2i food;
    sf::Font& font;
    sf::RenderWindow* window;

public:
    explicit GameController(sf::Font& gameFont, sf::RenderWindow* gameWindow) 
        : font(gameFont), window(gameWindow) {}
    
    void initializeGame();
    void handleInput(const sf::Event& event);
    void update();
    void render(sf::RenderWindow& window);
    void quitGame();
    
    StateMachine& getStateMachine() { return stateMachine; }
    const sf::Font& getFont() const { return font; }
};