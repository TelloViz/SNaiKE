#pragma once
#include <SFML/Graphics.hpp>
#include "GameResources.hpp"
#include "GameConfig.hpp"

class GameController;
class StateMachine;

/**
 * @brief Abstract base class for game states
 * 
 * Defines the interface for all game states and provides:
 * - Access to game controller and state machine
 * - Access to runtime resources
 * - Virtual methods for state lifecycle management
 * 
 * States represent different game modes (Menu, Playing, Paused, etc.)
 * and encapsulate the behavior and rendering for each mode.
 */
class State {
protected:
    GameController* gameController;  ///< Pointer to game controller
    StateMachine* stateMachine;     ///< Pointer to state machine
    const GameResources& resources; ///< Reference to game resources

public:
    /**
     * @brief Constructs a new state
     * @param controller Pointer to game controller
     * @param res Reference to game resources
     * @param machine Pointer to state machine
     */
    State(GameController* controller, const GameResources& res, StateMachine* machine)
        : gameController(controller)
        , resources(res)
        , stateMachine(machine) {}
    
    virtual ~State() = default;
    virtual void handleInput(const sf::Event& event) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual void pause() {}
    virtual void resume() {}
};