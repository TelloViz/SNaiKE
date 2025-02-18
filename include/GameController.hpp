#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.hpp"
#include "Snake.hpp"
#include "Input/InputHandler.hpp"  // Add this include

/**
 * @brief Controls game logic and manages state transitions
 * 
 * The GameController serves as the primary coordinator between:
 * - State management (through StateMachine)
 * - Game entities (Snake, Food)
 * - Resource management (Font, Window)
 * - Game loop delegation (Input, Update, Render)
 */
class GameController {
private:
    StateMachine stateMachine;    ///< Manages game state transitions
    Snake snake;                  ///< Main player entity // TODO Consider moving this to exclusively within playing state
    sf::Vector2i food;           ///< Current food position
    sf::Font& font;              ///< Reference to global font resource
    sf::RenderWindow* window;    ///< Pointer to game window for control

public:
    /**
     * @brief Constructs the game controller
     * @param gameFont Reference to the global font resource
     * @param gameWindow Pointer to the main game window
     */
    explicit GameController(sf::Font& gameFont, sf::RenderWindow* gameWindow) 
        : font(gameFont), window(gameWindow) {}
    
    /**
     * @brief Initializes the game state and resources
     * 
     * Sets up initial game state (usually MenuState) and
     * prepares game resources for use
     */
    void initializeGame();

    /**
     * @brief Processes a single input event
     * @param input Game input to process
     * 
     * Delegates event handling to current game state
     */
    void handleInput(const GameInput& input);

    /**
     * @brief Updates game logic for current frame
     * 
     * Processes state changes and updates current state
     */
    void update();

    /**
     * @brief Renders current game state
     * @param window SFML window to render to
     */
    void render(sf::RenderWindow& window);

    /**
     * @brief Closes the game window
     * 
     * Called when game should exit (e.g., from menu)
     */
    void quitGame();
    
    /**
     * @brief Gets reference to state machine
     * @return Reference to the game's state machine
     */
    StateMachine& getStateMachine() { return stateMachine; }

    /**
     * @brief Gets reference to global font
     * @return Const reference to the game's font
     */
    const sf::Font& getFont() const { return font; }

    State* getCurrentState() const { 
        return stateMachine.getCurrentState(); 
    }
};