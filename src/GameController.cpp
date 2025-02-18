#include "GameController.hpp"
#include "states/MenuState.hpp"
#include "GameConfig.hpp"
#include "input/InputHandler.hpp"
#include <iostream>


void GameController::initializeGame() {
    StateContext context{
        font,
        GameConfig::GRID_WIDTH,
        GameConfig::GRID_HEIGHT,
        GameConfig::CELL_SIZE,
        GameConfig::MARGIN_TOP,
        GameConfig::MARGIN_BOTTOM,
        GameConfig::MARGIN_SIDES,
        GameConfig::BORDER_THICKNESS
    };
    
    auto menuState = std::make_unique<MenuState>(this, context, &stateMachine);
    if (!menuState) {
        return;
    }
    
    stateMachine.pushState(std::move(menuState));
    
    // Process the state change immediately
    stateMachine.processStateChanges();
    
    // Verify state was pushed
    if (stateMachine.hasState()) {
        auto* state = stateMachine.getCurrentState();
    } else {
    }
}

void GameController::handleInput(const GameInput& input) {
    // Process exactly one input
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->handleInput(input);
    }
}

void GameController::update() {
    // Process one input per update if available
    if (inputHandler.hasInput()) {
        auto gameInput = inputHandler.getNextInput();
        if (stateMachine.hasState()) {
            stateMachine.getCurrentState()->handleInput(gameInput);
        }
    }

    // Update game state
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->update();
    }
}

void GameController::render(sf::RenderWindow& window) {
    static bool errorPrinted = false;  // Static flag to track if error was printed

    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->render(window);
        errorPrinted = false;  // Reset flag when we have a valid state
    } else {
        if (!errorPrinted) {
            std::cerr << "No valid game state to render!" << std::endl;
            errorPrinted = true;  // Set flag after printing once
        }
    }
}

void GameController::quitGame() {
    if (window) {
        window->close();
    }
}