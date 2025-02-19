#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "GameConfig.hpp"
#include <iostream>

PlayingState::PlayingState(GameController* ctrl, const StateContext& ctx, StateMachine* mach)
    : State(ctrl, ctx, mach)
    , controller(ctrl)
    , context(ctx)
    , machine(mach)
    , isFrozen(false)
    , score(0) {
    std::cout << "PlayingState constructed successfully" << std::endl;
    snake = Snake(ctx.width / 2, ctx.height / 2);
    rng = std::mt19937(std::random_device{}());
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
                std::cout << "Playing: Back pressed, pushing PausedState" << std::endl;
                machine->pushState(std::make_unique<PausedState>(controller, context, machine));
                machine->processStateChanges();  // Important: Process the state change immediately
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
                case GameButton::ToggleAI:
                aiControlled = !aiControlled;
                std::cout << "AI Control: " << (aiControlled ? "ON" : "OFF") << std::endl;
                if (aiControlled && !aiPlayer) {
                    aiPlayer = std::make_unique<AIPlayer>(snake, food);
                }
                break;
            case GameButton::Num1:
                if (aiPlayer) {
                    aiPlayer->setStrategy(AIStrategy::Basic);
                    std::cout << "AI Strategy: Basic" << std::endl;
                }
                break;
            case GameButton::Num2:
                if (aiPlayer) {
                    aiPlayer->setStrategy(AIStrategy::Advanced);
                    std::cout << "AI Strategy: Advanced" << std::endl;
                }
                break;
            case GameButton::Num3:
                if (aiPlayer) {
                    aiPlayer->setStrategy(AIStrategy::Random);
                    std::cout << "AI Strategy: Random" << std::endl;
                }
                break;
        }
    }
}

void PlayingState::update() {
    if (aiControlled && aiPlayer) {
        // Let AI generate input
        GameInput aiInput = aiPlayer->getNextInput();
        handleInput(aiInput);
    }
    sf::Time currentTime = gameTime.getElapsedTime();
    sf::Time moveInterval = sf::seconds(1.0f / GameConfig::SNAKE_SPEED);
    
    if (currentTime.asSeconds() - lastMoveTime.asSeconds() >= moveInterval.asSeconds()) {
        snake.move();
        lastMoveTime = currentTime;

        if (checkCollision()) {
            std::cout << "Collision detected! Transitioning to GameOver state..." << std::endl;
            machine->replaceState(std::make_unique<GameOverState>(controller, context, machine));
            machine->processStateChanges();
            return;
        }

        if (snake.getHead() == food) {
            snake.grow();
            score += 10;  // Increment score when food is eaten
            spawnFood();
        }
    }
}

bool PlayingState::checkCollision() {
    const sf::Vector2i& head = snake.getHead();
    
    // Wall collision
    if (head.x < 0 || head.x >= context.width ||
        head.y < 0 || head.y >= context.height) {
        std::cout << "Wall collision detected!" << std::endl;
        return true;
    }
    
    // Self collision
    const auto& body = snake.getBody();
    for (size_t i = 1; i < body.size(); ++i) {
        if (head == body[i]) {
            std::cout << "Self collision detected!" << std::endl;
            return true;
        }
    }
    
    return false;
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
    // Convert SNAKE_MOVE_INTERVAL to sf::Time before subtraction
    lastMoveTime = gameTime.getElapsedTime() - sf::seconds(SNAKE_MOVE_INTERVAL * 0.9f);
}