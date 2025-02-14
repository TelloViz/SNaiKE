#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "GameConfig.hpp"
#include "states/StateFactory.hpp"

PlayingState::PlayingState(GameController* controller, const GameResources& resources, StateMachine* machine)
    : State(controller, resources, machine)
    , snake(GameConfig::GRID_WIDTH / 2, GameConfig::GRID_HEIGHT / 2)
    , rng(std::random_device{}())
{
    spawnFood();
}

void PlayingState::spawnFood() {
    std::uniform_int_distribution<int> disX(0, GameConfig::GRID_WIDTH - 1);
    std::uniform_int_distribution<int> disY(0, GameConfig::GRID_HEIGHT - 1);
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
                    StateFactory::createState(StateType::Paused, gameController, resources, stateMachine)
                );
                break;
        }
    }
}

void PlayingState::update() {
    snake.move();
    
    if (snake.checkCollision(GameConfig::GRID_WIDTH, GameConfig::GRID_HEIGHT)) {
        stateMachine->replaceState(
            StateFactory::createState(StateType::GameOver, gameController, resources, stateMachine)
        );
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
        sf::RectangleShape segmentShape(sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE - 2));
        segmentShape.setPosition(segment.x * GameConfig::CELL_SIZE + 1, segment.y * GameConfig::CELL_SIZE + 1);
        segmentShape.setFillColor(sf::Color::Green);
        window.draw(segmentShape);
    }
    
    // Draw food
    sf::RectangleShape foodShape(sf::Vector2f(GameConfig::CELL_SIZE - 2, GameConfig::CELL_SIZE - 2));
    foodShape.setPosition(food.x * GameConfig::CELL_SIZE + 1, food.y * GameConfig::CELL_SIZE + 1);
    foodShape.setFillColor(sf::Color::Red);
    window.draw(foodShape);
}

void PlayingState::pause() {
    // Save game state or pause game logic if needed
}

void PlayingState::resume() {
    // Restore game state or resume game logic if needed
}