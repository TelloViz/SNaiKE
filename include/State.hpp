#pragma once
#include <SFML/Graphics.hpp>

class GameController;
class StateMachine;

/**
 * @brief Context struct containing shared game state data
 * 
 * Passed to states during construction to provide access
 * to common resources and configuration values.
 */
struct StateContext {
    const sf::Font& font;    ///< Reference to global font resource
    int width;              ///< Grid width in cells
    int height;             ///< Grid height in cells
    int cellSize;           ///< Size of each cell in pixels
};

/**
 * @brief Abstract base class for game states
 * 
 * Defines the interface for all game states and provides:
 * - Access to game controller and state machine
 * - Common state context data
 * - Virtual methods for state lifecycle management
 * 
 * States represent different game modes (Menu, Playing, Paused, etc.)
 * and encapsulate the behavior and rendering for each mode.
 */
class State {
protected:
    GameController* gameController;  ///< Pointer to game controller
    StateMachine* stateMachine;     ///< Pointer to state machine
    StateContext context;           ///< Shared state context data

public:
    /**
     * @brief Constructs a new state
     * @param controller Pointer to game controller
     * @param ctx Shared state context
     * @param machine Pointer to state machine
     */
    State(GameController* controller, const StateContext& ctx, StateMachine* machine)
        : gameController(controller), context(ctx), stateMachine(machine) {}
    
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~State() = default;

    /**
     * @brief Handles input events for the state
     * @param event SFML event to process
     */
    virtual void handleInput(const sf::Event& event) = 0;

    /**
     * @brief Updates state logic for current frame
     */
    virtual void update() = 0;

    /**
     * @brief Renders state to the window
     * @param window SFML window to render to
     */
    virtual void render(sf::RenderWindow& window) = 0;
    
    /**
     * @brief Called when state is paused (another state pushed)
     */
    virtual void pause() {} 

    /**
     * @brief Called when state is resumed (top state popped)
     */
    virtual void resume() {}
};