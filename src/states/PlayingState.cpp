#include "states/PlayingState.hpp"
#include "states/GameOverState.hpp"
#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "GameConfig.hpp"
#include "ScoreLogger.hpp"  // Add this line
#include <iostream>

PlayingState::PlayingState(GameController* ctrl, const StateContext& ctx, StateMachine* mach)
    : State(ctrl, ctx, mach)
    , controller(ctrl)
    , context(ctx)
    , machine(mach)
    , isFrozen(false)
    , score(0)
    , gameLength(0) {
    std::cout << "PlayingState constructed successfully" << std::endl;
    snake = Snake(ctx.width / 2, ctx.height / 2);
    rng = std::mt19937(std::random_device{}());
    spawnFood();
    gameTime.restart();  // Initialize the game clock
    lastMoveTime = gameTime.getElapsedTime();  // Initialize last move time
    lastStrategy = AIStrategy::Manhattan;  // Initialize with default strategy
    strategyChanges.push_back({lastStrategy, 0});  // Log initial strategy

        // Initialize algorithm label
        algoLabel.setFont(context.font);
        algoLabel.setCharacterSize(20);
        algoLabel.setFillColor(sf::Color::White);
        algoLabel.setPosition(
            GameConfig::WINDOW_WIDTH - 200, 
            10  // Just above the game border
        );
        updateAlgoLabel();
}

void PlayingState::updateAlgoLabel() {
    std::string algoText = "AI: ";
    if (aiControlled && aiPlayer) {
        if (auto* astar = dynamic_cast<AStarStrategy*>(aiPlayer->getCurrentStrategy())) {
            switch(astar->getHeuristic()) {
                case AStarStrategy::Heuristic::MANHATTAN:
                    algoText += "A* (Manhattan)";
                    break;
                case AStarStrategy::Heuristic::EUCLIDEAN:
                    algoText += "A* (Euclidean)";
                    break;
                case AStarStrategy::Heuristic::CHEBYSHEV:
                    algoText += "A* (Chebyshev)";
                    break;
            }
        } else if (dynamic_cast<ManhattanStrategy*>(aiPlayer->getCurrentStrategy())) {
            algoText += "Manhattan";
        } else {
            algoText += "None";
        }
    }
    algoLabel.setString(algoText);
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
                //updateAlgoLabel(); 
                std::cout << "AI Control: " << (aiControlled ? "ON" : "OFF") << std::endl;
                if (aiControlled && !aiPlayer) {
                    aiPlayer = std::make_unique<AIPlayer>(snake, food);
                }
                break;
            case GameButton::Num1: {
                // Store visualization states from either strategy type
                if (aiPlayer && aiPlayer->getCurrentStrategy()) {
                    if (auto* astar = dynamic_cast<AStarStrategy*>(aiPlayer->getCurrentStrategy())) {
                        lastHeatMapState = astar->isHeatMapEnabled();
                        lastPathArrowsState = astar->isPathArrowsEnabled();
                    } else if (auto* manhattan = dynamic_cast<ManhattanStrategy*>(aiPlayer->getCurrentStrategy())) {
                        // Also store heat map state from Manhattan strategy
                        lastHeatMapState = showHeatMap;  // Use the PlayingState's showHeatMap
                        lastPathArrowsState = false;  // Manhattan doesn't have path arrows
                    }
                }

                if (!aiPlayer) {
                    aiPlayer = std::make_unique<AIPlayer>(snake, food);
                }
                aiControlled = true;
                aiPlayer->setStrategy(AIStrategy::Manhattan);
                updateAlgoLabel();
                break;
            }
            
            case GameButton::Num2:
            case GameButton::Num3:
            case GameButton::Num4: {
                // Always preserve current visualization state before switching
                if (aiPlayer && aiPlayer->getCurrentStrategy()) {
                    if (auto* astar = dynamic_cast<AStarStrategy*>(aiPlayer->getCurrentStrategy())) {
                        lastHeatMapState = astar->isHeatMapEnabled();
                        lastPathArrowsState = astar->isPathArrowsEnabled();
                    }
                }

                if (!aiPlayer) {
                    aiPlayer = std::make_unique<AIPlayer>(snake, food);
                }
                aiControlled = true;
                aiPlayer->setStrategy(AIStrategy::AStar);

                if (auto* astar = dynamic_cast<AStarStrategy*>(aiPlayer->getCurrentStrategy())) {
                    switch (input.button) {
                        case GameButton::Num2:
                            astar->setHeuristic(AStarStrategy::Heuristic::MANHATTAN);
                            break;
                        case GameButton::Num3:
                            astar->setHeuristic(AStarStrategy::Heuristic::EUCLIDEAN);
                            break;
                        case GameButton::Num4:
                            astar->setHeuristic(AStarStrategy::Heuristic::CHEBYSHEV);
                            break;
                    }
                    astar->setVisualizationState(lastHeatMapState, lastPathArrowsState);
                }
                updateAlgoLabel();
                break;
            }
            case GameButton::H:  // Add new button for heat map toggle
                showHeatMap = !showHeatMap;
                std::cout << "Heat map display: " << (showHeatMap ? "ON" : "OFF") << std::endl;
                if (aiControlled && aiPlayer) {
                    if (auto* astar = dynamic_cast<AStarStrategy*>(aiPlayer->getCurrentStrategy())) {
                        astar->toggleHeatMap();
                    }
                    // Store state regardless of current strategy type
                    lastHeatMapState = showHeatMap;
                }
                break;
            case GameButton::J:
                if (aiControlled && aiPlayer) {
                    if (auto* astar = dynamic_cast<AStarStrategy*>(aiPlayer->getCurrentStrategy())) {
                        astar->togglePathArrows();
                        lastPathArrowsState = astar->isPathArrowsEnabled();
                    }
                }
                break;
        }
    }
}

void PlayingState::update() {
    gameLength++;
    if (!isFrozen) {


        // Check for strategy changes
        if (aiPlayer && aiPlayer->getStrategy() != lastStrategy) {
            strategyChanges.push_back({aiPlayer->getStrategy(), score});
            lastStrategy = aiPlayer->getStrategy();
        }

        if (aiControlled && aiPlayer) {
            GameInput aiInput = aiPlayer->getNextInput();
            handleInput(aiInput);
        }

        updateAlgoLabel(); 

        sf::Time currentTime = gameTime.getElapsedTime();
        sf::Time moveInterval = sf::seconds(1.0f / GameConfig::SNAKE_SPEED);
        
        if (currentTime.asSeconds() - lastMoveTime.asSeconds() >= moveInterval.asSeconds()) {
            snake.move();
            lastMoveTime = currentTime;

            // Check collisions after movement
            if (checkCollision()) {
                // Log the game data before transitioning
                ScoreLogger::GameSession session;
                session.finalScore = score;
                session.gameLength = gameLength;
                session.strategyChanges = strategyChanges;
                ScoreLogger::logGame(session);

                std::cout << "Game Over - Final Score: " << score 
                         << ", Length: " << gameLength 
                         << ", Strategy Changes: " << strategyChanges.size() << std::endl;

                // Transition to game over
                machine->replaceState(std::make_unique<GameOverState>(controller, context, machine, score));
                machine->processStateChanges();
                return;
            }

            if (snake.getHead() == food) {
                snake.grow();
                score += 10;
                spawnFood();
            }
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

void PlayingState::renderFood(sf::RenderWindow& window) const {
    sf::RectangleShape foodShape(sf::Vector2f(context.cellSize, context.cellSize));
    foodShape.setFillColor(sf::Color::Red);
    foodShape.setPosition(
        food.x * context.cellSize + context.marginSides,
        food.y * context.cellSize + context.marginTop
    );
    window.draw(foodShape);
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
    
    // Draw snake and food
    snake.render(window);
    renderFood(window);  // Use the new helper function

    // Draw Score (left side)
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

    // Draw AI Algorithm label (right side)
    algoLabel.setPosition(
        context.marginSides + context.width * context.cellSize - algoLabel.getGlobalBounds().width,
        context.marginTop / 2.0f - algoLabel.getGlobalBounds().height / 2.0f
    );
    window.draw(algoLabel);  // Add this line to actually draw the label

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

    // Heat map visualization with debug output
    if (showHeatMap && aiControlled && aiPlayer) {
        std::cout << "Attempting to render heat map...\n";
        const BaseStrategy* strategy = aiPlayer->getCurrentStrategy();
        
        if (const auto* basicStrat = dynamic_cast<const ManhattanStrategy*>(strategy)) {
            std::cout << "Found Manhattan Strategy, rendering heat map...\n";
            basicStrat->updateHeatMap(snake, food);  // Add this line
            basicStrat->render(window);
        }
        else if (const auto* astarStrat = dynamic_cast<const AStarStrategy*>(strategy)) {
            std::cout << "Found A* Strategy, rendering heat map...\n";
            astarStrat->render(window);
        }
        else {
            std::cout << "No compatible strategy found for heat map rendering\n";
        }
    }
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