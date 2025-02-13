#include "Game.hpp"
#include "states/States.hpp"
#include <random>
#include <iostream>
#include <filesystem>

Game::Game() 
    : window(sf::VideoMode(GameConfig::GRID_WIDTH * GameConfig::CELL_SIZE, 
                          GameConfig::GRID_HEIGHT * GameConfig::CELL_SIZE), "Snake Game")
    , gameController(font, &window) {
    window.setFramerateLimit(GameConfig::FRAME_RATE);
    
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
    gameController.render(window);
    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}