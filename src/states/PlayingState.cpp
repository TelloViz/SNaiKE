#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"

PlayingState::PlayingState(GameController* controller, const StateContext& context, StateMachine* machine)
    : State(controller, context, machine)
    , snake(context.width / 2, context.height / 2)
    , rng(std::random_device{}())
{
    spawnFood();
}

void PlayingState::spawnFood() {
    std::uniform_int_distribution<int> disX(0, context.width - 1);
    std::uniform_int_distribution<int> disY(0, context.height - 1);
    bool validPosition;
    do {
        food.x = disX(rng);
        food.y = disY(rng);
        validPosition = true;
        
        for (const auto& segment : snake.getBody()) {
            if (segment.x == food.x && segment.y == food.y) {
                validPosition = false;
                break;
            }
        }
    } while (!validPosition);
}

void PlayingState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up: snake.setDirection(Direction::Up); break;
            case sf::Keyboard::Down: snake.setDirection(Direction::Down); break;
            case sf::Keyboard::Left: snake.setDirection(Direction::Left); break;
            case sf::Keyboard::Right: snake.setDirection(Direction::Right); break;
            case sf::Keyboard::Escape:
                stateMachine->pushState(
                    std::make_unique<PausedState>(gameController, context, stateMachine));
                break;
        }
    }
}

void PlayingState::update() {
    snake.move();
    
    if (snake.checkCollision(context.width, context.height)) {
        stateMachine->replaceState(
            std::make_unique<GameOverState>(gameController, context, stateMachine));
        return;
    }

    if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
        snake.grow();
        spawnFood();
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    // Draw snake
    for (const auto& segment : snake.getBody()) {
        sf::RectangleShape segmentShape(sf::Vector2f(context.cellSize - 2, context.cellSize - 2));
        segmentShape.setPosition(segment.x * context.cellSize + 1, segment.y * context.cellSize + 1);
        segmentShape.setFillColor(sf::Color::Green);
        window.draw(segmentShape);
    }
    
    // Draw food
    sf::RectangleShape foodShape(sf::Vector2f(context.cellSize - 2, context.cellSize - 2));
    foodShape.setPosition(food.x * context.cellSize + 1, food.y * context.cellSize + 1);
    foodShape.setFillColor(sf::Color::Red);
    window.draw(foodShape);
}

void PlayingState::freeze() {
    // Save game state or freeze game logic if needed
}

void PlayingState::unfreeze() {
    // Restore game state or unfreeze game logic if needed
}