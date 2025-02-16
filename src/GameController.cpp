#include "GameController.hpp"
#include "states/MenuState.hpp"
#include "GameConfig.hpp"
#include "states/StateFactory.hpp"
#include "GameResources.hpp"

void GameController::initializeGame() {
    GameResources resources{font};  // Now only passing font since we removed config from GameResources
    
    stateMachine.replaceState(
        StateFactory::createState(StateType::Menu, this, resources, &stateMachine)
    );
}

void GameController::handleInput(const sf::Event& event) {
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->handleInput(event);
    }
}

void GameController::update() {
    stateMachine.processStateChanges();
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->update();
    }
}

void GameController::render(sf::RenderWindow& window) {
    if (stateMachine.hasState()) {
        stateMachine.getCurrentState()->render(window);
    }
}

void GameController::quitGame() {
    // Clear any states before closing
    stateMachine.clearStates();
    
    if (window) {
        window->close();
    }
}