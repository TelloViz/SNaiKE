#pragma once
#include <SFML/Graphics.hpp>
#include "GameController.hpp"
#include "GameConfig.hpp"

/**
 * @brief Main game class that handles the game window and core game loop
 * 
 * The Game class is responsible for:
 * - Creating and managing the SFML window
 * - Loading and managing global resources (fonts)
 * - Running the main game loop
 * - Delegating game logic to GameController
 */
class GameLoop {
private:
    sf::RenderWindow window;     ///< SFML window for rendering
    sf::Font font;              ///< Global font resource
    GameController gameController; ///< Manages game states and logic

public:
    /**
     * @brief Constructs the game object
     * 
     * Initializes the SFML window, loads resources,
     * and sets up the GameController
     */
    GameLoop();

    /**
     * @brief Starts and runs the main game loop
     * 
     * Handles the core game loop which:
     * 1. Processes events
     * 2. Updates game logic
     * 3. Renders the current frame
     */
    void run();

    /**
     * @brief Processes all pending SFML events
     * 
     * Handles window events and delegates game-specific
     * events to the GameController
     */
    void processEvents();

    /**
     * @brief Updates game logic for the current frame
     * 
     * Delegates update logic to GameController which
     * then updates the current game state
     */
    void update();

    /**
     * @brief Renders the current frame
     * 
     * Clears the window, delegates rendering to
     * GameController, and displays the frame
     */
    void render();
};