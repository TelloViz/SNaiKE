#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "GameConfig.hpp"
#include "StateMachine.hpp"

PausedState::PausedState(GameController* controller, const GameResources& resources, StateMachine* machine)
    : State(controller, resources, machine) {
    pausedText.setFont(resources.font);
    pausedText.setString("PAUSED");
    pausedText.setCharacterSize(50);
    pausedText.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = pausedText.getLocalBounds();
    pausedText.setPosition(
        (GameConfig::GRID_WIDTH * GameConfig::CELL_SIZE - textBounds.width) / 2,
        (GameConfig::GRID_HEIGHT * GameConfig::CELL_SIZE - textBounds.height) / 2
    );
}

void PausedState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        stateMachine->popState();
    }
}

void PausedState::update() {
    // Nothing to update in pause state
}

void PausedState::render(sf::RenderWindow& window) {
    window.draw(pausedText);
}