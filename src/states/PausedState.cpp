#include "states/PausedState.hpp"
#include "GameController.hpp"

PausedState::PausedState(GameController* controller, const StateContext& context, StateMachine* machine)
    : State(controller, context, machine) {
    pausedText.setFont(context.font);
    pausedText.setString("PAUSED");
    pausedText.setCharacterSize(50);
    pausedText.setFillColor(sf::Color::White);
    
    sf::FloatRect textBounds = pausedText.getLocalBounds();
    pausedText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        (context.height * context.cellSize - textBounds.height) / 2
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