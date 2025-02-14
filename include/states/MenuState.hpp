#pragma once
#include "../State.hpp"
#include <vector>

// Forward declarations
class GameController;
class StateMachine;

/**
 * @brief Main menu state of the game
 * 
 * Provides the main menu interface with:
 * - Game title
 * - Selectable menu options (Play, Quit)
 * - Keyboard navigation
 * - Visual feedback for selection
 */
class MenuState : public State {
private:
    sf::Text titleText;                ///< Game title text
    std::vector<sf::Text> menuOptions; ///< List of menu options
    int selectedOption;                ///< Currently selected menu item index

public:
    /**
     * @brief Constructs menu state
     * @param controller Pointer to game controller
     * @param context Shared state context
     * @param machine Pointer to state machine
     * 
     * Initializes menu elements and positions them on screen
     */
    MenuState(GameController* controller, const StateContext& context, StateMachine* machine);

    /**
     * @brief Handles input events
     * @param event SFML event to process
     * 
     * Processes:
     * - Up/Down arrows for menu navigation
     * - Enter to select option
     * - Updates visual feedback for selection
     */
    void handleInput(const sf::Event& event) override;

    /**
     * @brief Updates menu state
     * 
     * Currently empty as menu is static
     */
    void update() override;

    /**
     * @brief Renders menu screen
     * @param window SFML window to render to
     * 
     * Draws:
     * - Game title
     * - Menu options with selected item highlighted
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