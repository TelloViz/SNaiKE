#include "states/GameOverState.hpp"
#include "states/MenuState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"

GameOverState::GameOverState(GameController* controller, const StateContext& context, StateMachine* machine)
    : State(controller, context, machine) {
    
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
        stateMachine->replaceState(std::make_unique<MenuState>(gameController, context, stateMachine));
    }
}

void GameOverState::update() {
    // Nothing to update in game over state
}

void GameOverState::render(sf::RenderWindow& window) {
    window.draw(gameOverText);
    window.draw(scoreText);
}

void GameOverState::freeze() {
    // Nothing to freeze in game over state
}

void GameOverState::unfreeze() {
    // Nothing to unfreeze in game over state
}