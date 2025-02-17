#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"

PlayingState::PlayingState(GameController* ctrl, const StateContext& ctx, StateMachine* mach)
    : State(ctrl, ctx, mach)
    , controller(ctrl)
    , context(ctx)
    , machine(mach)
    , snake(ctx.width / 2, ctx.height / 2)
    , rng(std::random_device{}())
{
    spawnFood();
    gameTime.restart();  // Initialize the game clock
    lastMoveTime = gameTime.getElapsedTime();  // Initialize last move time
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
            case sf::Keyboard::Escape:
                freeze();  // Make sure we freeze before pushing pause state
                machine->pushState(std::make_unique<PausedState>(controller, context, machine));
                break;
                
            case sf::Keyboard::L:  // Changed from F1 to L
                break;
                
            // Handle other game inputs
            case sf::Keyboard::Up: snake.setDirection(Direction::Up); break;
            case sf::Keyboard::Down: snake.setDirection(Direction::Down); break;
            case sf::Keyboard::Left: snake.setDirection(Direction::Left); break;
            case sf::Keyboard::Right: snake.setDirection(Direction::Right); break;
        }
    }
}

void PlayingState::update() {
    if (!isFrozen) {
        // Update game logic only when not frozen
        float currentTime = gameTime.getElapsedTime();


        while (currentTime - lastMoveTime >= SNAKE_MOVE_INTERVAL) {
            snake.move();
            lastMoveTime += SNAKE_MOVE_INTERVAL;
        }
        
        if (snake.checkCollision(context.width, context.height)) {
            stateMachine->replaceState(
                std::make_unique<GameOverState>(gameController, context, stateMachine));
            return;
        }

        // Fix the food collision check (was comparing x with x instead of y)
        if (snake.getHead().x == food.x && snake.getHead().y == food.y) {  // Fixed y comparison
            snake.grow();
            spawnFood();
        }
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
    isFrozen = true;
    gameTime.freeze();
}

void PlayingState::unfreeze() {
    isFrozen = false;
    gameTime.unfreeze();
    // Reset lastMoveTime to current time minus a small offset
    // This ensures the snake moves shortly after unfreeze
    lastMoveTime = gameTime.getElapsedTime() - (SNAKE_MOVE_INTERVAL * 0.9f);
}