#include "GameStates.hpp"
#include "Game.hpp"
#include "Snake.hpp"

// MenuState::MenuState(Game* game, const StateContext& context) 
// : State(game, context), selectedOption(0) {
//     // Title setup
//     titleText.setFont(context.font);
//     titleText.setString("SNAKE GAME");
//     titleText.setCharacterSize(50);
//     titleText.setFillColor(sf::Color::Green);
    
//     // Center the title
//     sf::FloatRect textBounds = titleText.getLocalBounds();
//     titleText.setPosition(
//         (context.width * context.cellSize - textBounds.width) / 2,
//         context.height * context.cellSize * 0.2f
//     );

//     // Menu options setup
//     std::vector<std::string> options = {"Play", "Quit"};
//     for (size_t i = 0; i < options.size(); ++i) {
//         sf::Text option;
//         option.setFont(context.font);
//         option.setString(options[i]);
//         option.setCharacterSize(30);
//         option.setFillColor(sf::Color::White);
        
//         // Center each option
//         textBounds = option.getLocalBounds();
//         option.setPosition(
//             (context.width * context.cellSize - textBounds.width) / 2,
//             context.height * context.cellSize * (0.4f + i * 0.1f)
//         );
        
//         menuOptions.push_back(option);
//     }
    
//     // Highlight initial selection
//     menuOptions[selectedOption].setFillColor(sf::Color::Green);
// }

// void MenuState::handleInput(const sf::Event& event) {
//     if (event.type == sf::Event::KeyPressed) {
//         switch (event.key.code) {
//             case sf::Keyboard::Up:
//                 menuOptions[selectedOption].setFillColor(sf::Color::White);
//                 selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
//                 menuOptions[selectedOption].setFillColor(sf::Color::Green);
//                 break;
                
//             case sf::Keyboard::Down:
//                 menuOptions[selectedOption].setFillColor(sf::Color::White);
//                 selectedOption = (selectedOption + 1) % menuOptions.size();
//                 menuOptions[selectedOption].setFillColor(sf::Color::Green);
//                 break;
                
//             case sf::Keyboard::Enter:
//                 if (selectedOption == 0) {
//                     game->changeState(std::make_unique<PlayingState>(game, context));
//                 } else if (selectedOption == 1) {
//                     game->quit(); // We'll need to add this method
//                 }
//                 break;
//         }
//     }
// }

// void MenuState::update() {
//     // Update menu animations if any
// }

// void MenuState::render(sf::RenderWindow& window) {
//     window.clear(sf::Color::Black);
//     window.draw(titleText);
//     for (const auto& option : menuOptions) {
//         window.draw(option);
//     }
//     window.display();
// }

PlayingState::PlayingState(Game* game, const StateContext& context) 
    : State(game, context)
    , snake(context.width / 2, context.height / 2) // Initialize snake at center
    , rng(std::random_device{}()) // Initialize random number generator
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
        
        // Check if food spawned on snake body
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
    
    // Check for collision with walls
    if (snake.checkCollision(context.width, context.height)) {
        game->changeState(std::make_unique<GameOverState>(game, context));
        return;
    }

    // Check for collision with food
    if (snake.getHead().x == food.x && snake.getHead().y == food.y) {
        snake.grow();
        spawnFood();
    }

}

void PlayingState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    
    // Draw snake segments as squares
    for (const auto& pos : snake.getBody()) {
        sf::RectangleShape segment(sf::Vector2f(context.cellSize - 2, context.cellSize - 2));
        segment.setPosition(pos.x * context.cellSize + 1, pos.y * context.cellSize + 1);
        segment.setFillColor(sf::Color::Green);
        window.draw(segment);
    }
    
    // Draw food as a square
    sf::RectangleShape foodShape(sf::Vector2f(context.cellSize - 2, context.cellSize - 2));
    foodShape.setPosition(food.x * context.cellSize + 1, food.y * context.cellSize + 1);
    foodShape.setFillColor(sf::Color::Red);
    window.draw(foodShape);
    
    window.display();
}

PausedState::PausedState(Game* game, const StateContext& context) : State(game, context) {
    // pausedText.setFont(context.font);
    // pausedText.setString("PAUSED");
    // pausedText.setCharacterSize(50);
    // // Center using context dimensions
    // sf::FloatRect textBounds = pausedText.getLocalBounds();
    // pausedText.setPosition(
    //     (context.width * context.cellSize - textBounds.width) / 2,
    //     (context.height * context.cellSize - textBounds.height) / 2
    // );
}

void PausedState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        game->changeState(std::make_unique<PlayingState>(game, context));
    }
}

void PausedState::update() {
    // Nothing to update while paused
}

void PausedState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    
    // Draw a pause symbol (two vertical rectangles)
    sf::RectangleShape pause1(sf::Vector2f(30, 100));
    sf::RectangleShape pause2(sf::Vector2f(30, 100));
    pause1.setFillColor(sf::Color::White);
    pause2.setFillColor(sf::Color::White);
    
    pause1.setPosition((context.width * context.cellSize - 80) / 2, 
                      (context.height * context.cellSize - 100) / 2);
    pause2.setPosition((context.width * context.cellSize + 20) / 2, 
                      (context.height * context.cellSize - 100) / 2);
    
    window.draw(pause1);
    window.draw(pause2);
    window.display();
}

GameOverState::GameOverState(Game* game, const StateContext& context) : State(game, context) {
    // gameOverText.setFont(context.font);
    // gameOverText.setString("GAME OVER");
    // gameOverText.setCharacterSize(50);
    // // Center using context dimensions
    // sf::FloatRect textBounds = gameOverText.getLocalBounds();
    // gameOverText.setPosition(
    //     (context.width * context.cellSize - textBounds.width) / 2,
    //     (context.height * context.cellSize - textBounds.height) / 2
    // );
    // scoreText.setFont(game->getFont());
}

void GameOverState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
        game->changeState(std::make_unique<MenuState>(game, context));
    }
}

void GameOverState::update() {
    // Nothing to update in game over state
}

void GameOverState::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    
    // Draw an X shape for game over
    sf::RectangleShape cross1(sf::Vector2f(200, 20));
    sf::RectangleShape cross2(sf::Vector2f(200, 20));
    cross1.setFillColor(sf::Color::Red);
    cross2.setFillColor(sf::Color::Red);
    
    cross1.setOrigin(100, 10);
    cross2.setOrigin(100, 10);
    
    float centerX = context.width * context.cellSize / 2;
    float centerY = context.height * context.cellSize / 2;
    
    cross1.setPosition(centerX, centerY);
    cross2.setPosition(centerX, centerY);
    cross1.setRotation(45);
    cross2.setRotation(-45);
    
    window.draw(cross1);
    window.draw(cross2);
    window.display();
}
