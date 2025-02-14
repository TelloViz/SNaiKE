#pragma once
#include "../State.hpp"

// Forward declarations
class GameController;
class StateMachine;

/**
 * @brief Game over screen state
 * 
 * Displays when the player loses and shows:
 * - Game over message
 * - Final score
 * - Press Enter to return to menu prompt
 */
class GameOverState : public State {
private:
    sf::Text gameOverText;    ///< Main "Game Over" message
    sf::Text scoreText;       ///< Displays final score

public:
    /**
     * @brief Constructs game over state
     * @param controller Pointer to game controller
     * @param context Shared state context
     * @param machine Pointer to state machine
     * 
     * Initializes text elements and positions them on screen
     */
    GameOverState(GameController* controller, const StateContext& context, StateMachine* machine);

    /**
     * @brief Handles input events
     * @param event SFML event to process
     * 
     * Processes:
     * - Enter key to return to menu
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Updates game over state
     * 
     * Currently empty as state is static
     */
    void update() override;

    /**
     * @brief Renders game over screen
     * @param window SFML window to render to
     * 
     * Draws:
     * - Game over message
     * - Final score
     */
    void render(sf::RenderWindow& window) override;

    /**
     * @brief Freezes the game state
     * 
     */
    void freeze() override;

    /**
     * @brief Unfreezes the game state
     * 
     */
    void unfreeze() override;
};