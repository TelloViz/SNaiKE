#pragma once
#include <SFML/Graphics.hpp>

class GameController;
class StateMachine;


// Contextual information for the state
// This struct is passed to each state to provide necessary information
struct StateContext {
    const sf::Font& font;
    int width;
    int height;
    int cellSize;
};

class State {
protected:
    GameController* gameController;
    StateMachine* stateMachine;
    StateContext context;

public:
    State(GameController* controller, const StateContext& ctx, StateMachine* machine)
        : gameController(controller), context(ctx), stateMachine(machine) {}
    
    virtual ~State() = default;

    // Game Loop Functions
    virtual void handleInput(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    // Pause State (Not Game)
    virtual void pause() {} 

    // Resume State (Not Game)
    virtual void resume() {}
};