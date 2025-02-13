#pragma once
#include <SFML/Graphics.hpp>
#include "GameConfig.hpp"

class Game;

struct StateContext {
    const sf::Font& font;
    int width;
    int height;
    int cellSize;
};

class State {
protected:
    Game* game;
    StateContext context;

public:
    State(Game* game, const StateContext& context) 
        : game(game), context(context) {}
    virtual ~State() = default;

    virtual void handleInput(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // Add virtual methods for state management
    virtual void pause() {} 
    virtual void resume() {}
};