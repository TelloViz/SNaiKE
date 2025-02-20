#include "states/GameOverState.hpp"
#include "states/MenuState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "ScoreLogger.hpp"  // Add this include

GameOverState::GameOverState(GameController* ctrl, const StateContext& ctx, StateMachine* mach, int score)
    : State(ctrl, ctx, mach)
    , controller(ctrl)
    , context(ctx)
    , machine(mach)
    , finalScore(score)
{
    std::cout << "GameOverState constructed with score: " << score << std::endl;
    
    gameOverText.setFont(context.font);
    gameOverText.setString("GAME OVER\nPress ENTER to return to menu");
    gameOverText.setCharacterSize(30);
    gameOverText.setFillColor(sf::Color::Red);
    
    // Center the game over text
    sf::FloatRect textBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        (context.height * context.cellSize - textBounds.height) / 2 - 30 // Move up slightly
    );

    // Setup score text
    scoreText.setFont(context.font);
    scoreText.setString("Final Score: " + std::to_string(finalScore));
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    
    // Center the score text below game over text
    textBounds = scoreText.getLocalBounds();
    scoreText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        gameOverText.getPosition().y + gameOverText.getLocalBounds().height + 20
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