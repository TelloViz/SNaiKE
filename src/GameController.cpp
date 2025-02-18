#include "GameController.hpp"
#include "states/MenuState.hpp"
#include "GameConfig.hpp"


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
    static bool firstUpdate = true;
    if (firstUpdate) {
        firstUpdate = false;
    }

    if (stateMachine.hasState()) {
        // Process state changes before updating current state
        stateMachine.processStateChanges();
        
        auto* currentState = stateMachine.getCurrentState();
        if (currentState) {
            try {
                currentState->update();
            } catch (const std::exception& e) {
            }
        } else {
        }
    }
}

void GameController::render(sf::RenderWindow& window) {
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->render(window);
    }
}

void GameController::quitGame() {
    if (window) {
        window->close();
    }
}