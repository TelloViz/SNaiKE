#include "states/GameOverState.hpp"
#include "states/MenuState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"

GameOverState::GameOverState(GameController* ctrl, const StateContext& ctx, StateMachine* mach)
    : State(ctrl, ctx, mach)
    , controller(ctrl)
    , context(ctx)
    , machine(mach) {
    std::cout << "GameOverState constructed" << std::endl;
    
    gameOverText.setFont(context.font);
    gameOverText.setString("GAME OVER\nPress ENTER to return to menu");
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(sf::Color::Red);
    
    // Center the text
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        (context.height * context.cellSize - textBounds.height) / 2
    );
}

void GameOverState::handleInput(const GameInput& input) {
    if (input.type == InputType::ButtonPressed && input.button == GameButton::Select) {
        std::cout << "GameOver: Select pressed, returning to menu" << std::endl;
        machine->replaceState(std::make_unique<MenuState>(controller, context, machine));
        machine->processStateChanges();
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