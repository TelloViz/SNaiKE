#include "GameLoop.hpp"
#include "states/States.hpp"
#include "Input/InputHandler.hpp"
#include <random>
#include <iostream>
#include <filesystem>


GameLoop::GameLoop() 
    : window(sf::VideoMode(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT), "Snake Game")
    , gameController(font, &window) {

    // Get executable path and construct relative resource path
    std::filesystem::path exePath = std::filesystem::current_path() / "build" / "bin";
    std::filesystem::path fontPath = exePath / "resources" / "fonts" / "arial.ttf";
    
    //std::cout << "Executable path: " << exePath << std::endl;
    //std::cout << "Trying to load font from: " << fontPath << std::endl;
    
    if (!font.loadFromFile(fontPath.string())) {
        // Try fallback to relative path
        fontPath = "resources/fonts/arial.ttf";
        if (!font.loadFromFile(fontPath.string())) {
            std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
            throw std::runtime_error("Failed to load font from: " + fontPath.string());
        }
        }
    
    gameController.initializeGame();
}

void GameLoop::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            GameInput gameInput{InputType::WindowClosed, GameButton::Quit};
            inputHandler.pushEvent(gameInput);
        }
        else if (event.type == sf::Event::KeyPressed) {
            GameInput gameInput;
            gameInput.type = InputType::ButtonPressed;
            
            // Map SFML keys to game buttons
            switch (event.key.code) {
                case sf::Keyboard::Up:
                    gameInput.button = GameButton::Up;
                    break;
                case sf::Keyboard::Down:
                    gameInput.button = GameButton::Down;
                    break;
                case sf::Keyboard::Left:
                    gameInput.button = GameButton::Left;
                    break;
                case sf::Keyboard::Right:
                    gameInput.button = GameButton::Right;
                    break;
                case sf::Keyboard::Return:
                    gameInput.button = GameButton::Select;
                    break;
                case sf::Keyboard::Escape:
                    gameInput.button = GameButton::Back;
                    break;
                default:
                    continue; // Skip pushing event for unmapped keys
            }
            
            inputHandler.pushEvent(gameInput);
        }
    }

    while (inputHandler.hasInput()) {
        auto gameInput = inputHandler.getNextInput();
        gameController.handleInput(gameInput);
    }
}

void GameLoop::update() {
    gameController.update();
}

void GameLoop::render() {
    window.clear(sf::Color::Black);  // Use dot instead of arrow
    
    auto currentState = gameController.getCurrentState();
    if (currentState) {
        currentState->render(window);
        
    } else {
        std::cerr << "No valid game state to render!" << std::endl;
    }
    
    window.display();  // Use dot instead of arrow
}

void GameLoop::run() {
    const sf::Time timePerFrame = sf::seconds(1.0f / GameConfig::FRAME_RATE);
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (window.isOpen()) {
        processEvents();
        
        timeSinceLastUpdate += clock.restart();
        
        while (timeSinceLastUpdate > timePerFrame) {
            timeSinceLastUpdate -= timePerFrame;
            update();
        }
        
        render();
        
        window.setVerticalSyncEnabled(true);  // Enable VSync
    }
}