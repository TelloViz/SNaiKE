#pragma once
#include <SFML/Graphics.hpp>
#include "GameController.hpp"
#include "GameConfig.hpp"
#include "Input/InputHandler.hpp"

/**
 * @class GameLoop
 * @brief Main game loop handler that manages the core game execution
 *
 * The GameLoop class is responsible for:
 * - Managing the main SFML window
 * - Controlling the game's execution flow
 * - Processing input events
 * - Coordinating updates and rendering
 * - Managing global resources like fonts
 *
 * This class implements the standard game loop pattern:
 * 1. Process Events
 * 2. Update Game Logic
 * 3. Render Frame
 *
 * The class works closely with GameController to delegate
 * game-specific logic and state management.
 */
class GameLoop
{
private:
    sf::RenderWindow window;       ///< SFML window for rendering
    sf::Font font;                 ///< Global font resource
    InputHandler inputHandler;     ///< Handles input events
    GameController gameController; ///< Manages game states and logic

public:
    GameLoop();
    void run();
    void processEvents();
    void update();
    void render();
};