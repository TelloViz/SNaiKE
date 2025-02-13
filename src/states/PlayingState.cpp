#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "Game.hpp"

PlayingState::PlayingState(Game* game, const StateContext& context) 
    : State(game, context)
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
                game->changeState(std::make_unique<PausedState>(game, context));
                break;
        }
    }
}

void PlayingState::update() {
    snake.move();
    
    if (snake.checkCollision(context.width, context.height)) {
        game->changeState(std::make_unique<GameOverState>(game, context));
        return;
    }

    if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
        snake.grow();
        spawnFood();
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    
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
    
    window.display();
}