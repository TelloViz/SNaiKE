#pragma once
#include "../State.hpp"
#include "../Snake.hpp"
#include <random>

// Forward declarations
class GameController;
class StateMachine;

/**
 * @brief Main gameplay state
 * 
 * Manages the active gameplay including:
 * - Snake movement and growth
 * - Food spawning and collection
 * - Collision detection
 * - Score tracking
 * - Game over conditions
 */
class PlayingState : public State {
private:
    Snake snake;              ///< Player-controlled snake entity
    sf::Vector2i food;       ///< Current food position
    std::mt19937 rng;        ///< Random number generator for food placement

    /**
     * @brief Spawns new food in valid position
     * 
     * Places food:
     * - Randomly on grid
     * - Not on snake body
     * - Within grid bounds
     */
    void spawnFood();

public:
    /**
     * @brief Constructs playing state
     * @param controller Pointer to game controller
     * @param context Shared state context
     * @param machine Pointer to state machine
     * 
     * Initializes:
     * - Snake at starting position
     * - Random number generator
     * - Initial food placement
     */
    PlayingState(GameController* controller, const StateContext& context, StateMachine* machine);

    /**
     * @brief Handles input events
     * @param event SFML event to process
     * 
     * Processes:
     * - Arrow keys for snake direction
     * - Escape for pause
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Updates game logic
     * 
     * Handles:
     * - Snake movement
     * - Collision detection
     * - Food collection
     * - Game over conditions
     */
    void update() override;

    /**
     * @brief Renders game elements
     * @param window SFML window to render to
     * 
     * Draws:
     * - Snake body
     * - Food
     * - Score (if implemented)
     */
    void render(sf::RenderWindow& window) override;

    /**
     * @brief Freezes the game state
     * 
     * Preserves game state, stops snake movement, and pauses game timer if any
     */
    void freeze() override;

    /**
     * @brief Unfreezes the game state
     * 
     * Restores game state, resumes snake movement, and resumes game timer if any
     */
    void unfreeze() override;
};