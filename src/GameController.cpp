#include "GameController.hpp"
#include "states/MenuState.hpp"
#include "GameConfig.hpp"

void GameController::initializeGame() {
    StateContext context{
        font,
        GameConfig::GRID_WIDTH,
        GameConfig::GRID_HEIGHT,
        GameConfig::CELL_SIZE
    };
    
    stateMachine.replaceState(std::make_unique<MenuState>(this, context, &stateMachine));
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
    if (window) {
        window->close();
    }
}