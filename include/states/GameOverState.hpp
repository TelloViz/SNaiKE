#pragma once
#include "../State.hpp"
#include "../GameResources.hpp"

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
     * @param resources Reference to game resources
     * @param machine Pointer to state machine
     * 
     * Initializes text elements and positions them on screen
     */
    GameOverState(GameController* controller, const GameResources& resources, StateMachine* machine);

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
     * @brief Called when state is paused
     * 
     * Currently empty as state cannot be paused
     */
    void pause() override;

    /**
     * @brief Called when state is resumed
     * 
     * Currently empty as state cannot be resumed
     */
    void resume() override;
};