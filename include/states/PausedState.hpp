#pragma once
#include "../State.hpp"

// Forward declarations
class GameController;
class StateMachine;

/**
 * @brief Pause screen overlay state
 * 
 * Provides pause functionality with:
 * - Centered pause message
 * - Game state preservation
 * - Resume with Escape key
 * - Semi-transparent overlay
 */
class PausedState : public State {
private:
    sf::Text pausedText;    ///< Centered "PAUSED" message

public:
    /**
     * @brief Constructs pause state
     * @param controller Pointer to game controller
     * @param context Shared state context
     * @param machine Pointer to state machine
     * 
     * Initializes pause message and positions it centrally
     */
    PausedState(GameController* controller, const StateContext& context, StateMachine* machine);

    /**
     * @brief Handles input events
     * @param event SFML event to process
     * 
     * Processes:
     * - Escape key to resume game
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Updates pause state
     * 
     * Empty as state is static and underlying game is paused
     */
    void update() override;

    /**
     * @brief Renders pause overlay
     * @param window SFML window to render to
     * 
     * Draws:
     * - Centered pause message
     * - Optional: semi-transparent overlay
     */
    void render(sf::RenderWindow& window) override;
};