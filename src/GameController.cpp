#include "GameController.hpp"
#include "states/MenuState.hpp"
#include "GameConfig.hpp"
#include "debug/DebugOverlay.hpp"
#include "debug/DebugLogger.hpp"

void GameController::initializeGame() {
    DebugLogger::log("=== Initializing Game ===");
    
    StateContext context{
        font,
        GameConfig::GRID_WIDTH,
        GameConfig::GRID_HEIGHT,
        GameConfig::CELL_SIZE
    };
    
    auto menuState = std::make_unique<MenuState>(this, context, &stateMachine);
    if (!menuState) {
        DebugLogger::log("ERROR: Failed to create MenuState");
        return;
    }
    
    DebugLogger::log("Created MenuState successfully");
    stateMachine.pushState(std::move(menuState));
    DebugLogger::log("Pushed initial state");
    
    // Process the state change immediately
    stateMachine.processStateChanges();
    
    // Verify state was pushed
    if (stateMachine.hasState()) {
        DebugLogger::log("Initial state verified in stack");
        auto* state = stateMachine.getCurrentState();
        DebugLogger::log("Current state: " + (state ? state->getStateName() : "nullptr"));
        DebugOverlay::getInstance().setValue("Stack Status", "Initialized with " + state->getStateName());
    } else {
        DebugLogger::log("ERROR: Stack empty after push!");
        DebugOverlay::getInstance().setValue("Stack Status", "Empty after init");
    }
}

void GameController::handleInput(const sf::Event& event) {
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->handleInput(event);
    }
}

void GameController::update() {
    static bool firstUpdate = true;
    if (firstUpdate) {
        DebugLogger::log("=== First Update Cycle ===");
        DebugLogger::log("Has State: " + std::string(stateMachine.hasState() ? "Yes" : "No"));
        firstUpdate = false;
    }

    if (stateMachine.hasState()) {
        // Process state changes before updating current state
        stateMachine.processStateChanges();
        
        auto* currentState = stateMachine.getCurrentState();
        if (currentState) {
            try {
                DebugLogger::log("Updating state: " + currentState->getStateName());
                currentState->update();
            } catch (const std::exception& e) {
                DebugLogger::log("Exception in state update: " + std::string(e.what()));
                DebugOverlay::getInstance().setValue("Error", e.what());
            }
        } else {
            DebugLogger::log("ERROR: hasState() true but getCurrentState() returned nullptr");
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