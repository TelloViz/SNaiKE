#include "states/GameOverState.hpp"
#include "states/MenuState.hpp"
#include "Game.hpp"

GameOverState::GameOverState(Game* game, const StateContext& context) 
    : State(game, context) {
    gameOverText.setFont(context.font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        (context.height * context.cellSize - textBounds.height) / 2
    );
}

void GameOverState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        game->changeState(std::make_unique<MenuState>(game, context));
    }
}

void GameOverState::update() {
    // Nothing to update in game over state
}

void GameOverState::render(sf::RenderWindow& window) {
    window.draw(gameOverText);
}

void GameOverState::pause() {
    // Game over state pause implementation (can be empty)
}

void GameOverState::resume() {
    // Game over state resume implementation (can be empty)
}