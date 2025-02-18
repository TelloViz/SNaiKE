#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "GameConfig.hpp"

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

void PlayingState::handleInput(const GameInput& input) {
    if (input.type == InputType::ButtonPressed) {
        switch (input.button) {
            case GameButton::Back:
                freeze();  // Make sure we freeze before pushing pause state
                machine->pushState(std::make_unique<PausedState>(controller, context, machine));
                break;
            case GameButton::Up: 
                snake.setDirection(Direction::Up); 
                break;
            case GameButton::Down: 
                snake.setDirection(Direction::Down); 
                break;
            case GameButton::Left: 
                snake.setDirection(Direction::Left); 
                break;
            case GameButton::Right: 
                snake.setDirection(Direction::Right); 
                break;
        }
    }
}

void PlayingState::update() {
    if (!isFrozen) {
        float currentTime = gameTime.getElapsedTime();

        while (currentTime - lastMoveTime >= SNAKE_MOVE_INTERVAL) {
            snake.move();
            lastMoveTime += SNAKE_MOVE_INTERVAL;
            
            // Check all collisions using single method
            if (snake.checkCollision(context.width, context.height)) {
                stateMachine->replaceState(
                    std::make_unique<GameOverState>(gameController, context, stateMachine));
                return;
            }

            // Check food collision
            if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
                snake.grow();
                score += 10;
                spawnFood();
            }
        }
    }
}

void PlayingState::render(sf::RenderWindow& window) {
    // Draw border around gameplay area
    sf::RectangleShape border;
    border.setPosition(context.marginSides - context.borderThickness,
                      context.marginTop - context.borderThickness);
    border.setSize(sf::Vector2f(context.width * context.cellSize + context.borderThickness * 2,
                               context.height * context.cellSize + context.borderThickness * 2));
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::White);
    border.setOutlineThickness(context.borderThickness);
    window.draw(border);
    
    // Draw snake
    for (const auto& segment : snake.getBody()) {
        sf::RectangleShape segmentShape(sf::Vector2f(context.cellSize - 2, context.cellSize - 2));
        segmentShape.setPosition(
            context.marginSides + segment.x * context.cellSize + 1,
            context.marginTop + segment.y * context.cellSize + 1
        );
        segmentShape.setFillColor(sf::Color::Green);
        window.draw(segmentShape);
    }
    
    // Draw food
    sf::RectangleShape foodShape(sf::Vector2f(context.cellSize - 2, context.cellSize - 2));
    foodShape.setPosition(
        context.marginSides + food.x * context.cellSize + 1,
        context.marginTop + food.y * context.cellSize + 1
    );
    foodShape.setFillColor(sf::Color::Red);
    window.draw(foodShape);

    // Draw Score
    sf::Text scoreText;
    scoreText.setFont(context.font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: " + std::to_string(score));
    scoreText.setPosition(
        context.marginSides,
        context.marginTop / 2.0f - scoreText.getGlobalBounds().height / 2.0f
    );
    window.draw(scoreText);

    // Draw FPS (updated every frame)
    static sf::Clock fpsClock;
    static float fpsUpdateTime = 0;
    static int frameCount = 0;
    static float currentFps = 0;
    
    frameCount++;
    if (fpsClock.getElapsedTime().asSeconds() >= 0.5f) { // Update FPS every 0.5 seconds
        currentFps = frameCount / fpsClock.getElapsedTime().asSeconds();
        frameCount = 0;
        fpsClock.restart();
    }

    sf::Text fpsText;
    fpsText.setFont(context.font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setString("FPS: " + std::to_string(static_cast<int>(currentFps)));
    fpsText.setPosition(
        context.marginSides + context.width * context.cellSize - fpsText.getGlobalBounds().width,
        context.marginTop + context.height * context.cellSize + context.marginBottom / 2.0f - 
            fpsText.getGlobalBounds().height / 2.0f
    );
    window.draw(fpsText);
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