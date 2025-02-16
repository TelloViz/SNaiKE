#include "Game.hpp"
#include "states/States.hpp"
#include <random>
#include <iostream>
#include <filesystem>
#include "debug/DebugOverlay.hpp"
#include "debug/DebugLogger.hpp"

Game::Game() 
    : window(sf::VideoMode(GameConfig::GRID_WIDTH * GameConfig::CELL_SIZE, 
                          GameConfig::GRID_HEIGHT * GameConfig::CELL_SIZE), "Snake Game")
    , gameController(font, &window) {
    // Enable debug logging only in debug builds
    #ifdef _DEBUG
    DebugLogger::setEnabled(true);
    #endif

    window.setFramerateLimit(GameConfig::FRAME_RATE);
    DebugOverlay::getInstance().initialize(&window);
    DebugOverlay::getInstance().setValue("Game", "Initializing...");
    
    // Get executable path and construct relative resource path
    std::filesystem::path exePath = std::filesystem::current_path() / "build" / "bin";
    std::filesystem::path fontPath = exePath / "resources" / "fonts" / "arial.ttf";
    
    std::cout << "Executable path: " << exePath << std::endl;
    std::cout << "Trying to load font from: " << fontPath << std::endl;
    
    if (!font.loadFromFile(fontPath.string())) {
        // Try fallback to relative path
        fontPath = "resources/fonts/arial.ttf";
        if (!font.loadFromFile(fontPath.string())) {
            std::cout << "Current path: " << std::filesystem::current_path() << std::endl;
            throw std::runtime_error("Failed to load font from: " + fontPath.string());
        }
        DebugOverlay::getInstance().setValue("Error", "Failed to load font!");
    }
    
    gameController.initializeGame();
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        gameController.handleInput(event);
    }
}

void Game::update() {
    gameController.update();
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    auto currentState = gameController.getCurrentState();
    if (currentState) {
        DebugOverlay::getInstance().setValue("Current State", 
            currentState->getStateName());
        currentState->render(window);
        DebugOverlay::getInstance().setValue("Render", "State rendered");
    } else {
        DebugOverlay::getInstance().setValue("Render", "No state to render!");
        DebugOverlay::getInstance().setValue("Current State", "None");
    }
    
    window.display();
}

void Game::run() {
    DebugOverlay::getInstance().setValue("Game", "Running...");
    
    while (window.isOpen()) {
        processEvents();
        update();
        render();
        
        // Update and render debug overlay
        DebugOverlay::getInstance().update();
        DebugOverlay::getInstance().render();
    }
}