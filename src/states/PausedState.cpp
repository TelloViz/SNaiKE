#include "states/PausedState.hpp"
#include "states/PlayingState.hpp"
#include "Game.hpp"

PausedState::PausedState(Game* game, const StateContext& context) 
    : State(game, context) {
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
        game->changeState(std::make_unique<PlayingState>(game, context));
    }
}

void PausedState::update() {
    // Nothing to update in pause state
}

void PausedState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(pausedText);
    window.display();
}