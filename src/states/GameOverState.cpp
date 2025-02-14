#include "states/GameOverState.hpp"
#include "states/MenuState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "GameConfig.hpp"
#include "states/StateFactory.hpp"

GameOverState::GameOverState(GameController* controller, const GameResources& resources, StateMachine* machine)
    : State(controller, resources, machine) {
    
    gameOverText.setFont(resources.font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(
        (GameConfig::GRID_WIDTH * GameConfig::CELL_SIZE - textBounds.width) / 2,
        (GameConfig::GRID_HEIGHT * GameConfig::CELL_SIZE - textBounds.height) / 2
    );
}

void GameOverState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        stateMachine->replaceState(
            StateFactory::createState(StateType::Menu, gameController, resources, stateMachine)
        );
    }
}

void GameOverState::update() {
    // Nothing to update in game over state
}

void GameOverState::render(sf::RenderWindow& window) {
    window.draw(gameOverText);
    window.draw(scoreText);
}

void GameOverState::pause() {
    // Nothing to pause in game over state
}

void GameOverState::resume() {
    // Nothing to resume in game over state
}