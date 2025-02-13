#include "Game.hpp"
#include "GameStates.hpp"
#include <random>
#include <iostream>
#include <filesystem>


Game::Game() 
    : window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Snake Game") {
    window.setFramerateLimit(10);
    
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
    
    StateContext context{
        font,
        GameConfig::GRID_WIDTH,
        GameConfig::GRID_HEIGHT,
        GameConfig::CELL_SIZE
    };
    
    currentState = std::make_unique<MenuState>(this, context);
}

void Game::changeState(std::unique_ptr<State> newState) {
    currentState = std::move(newState);
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        currentState->handleInput(event);
    }
}

void Game::update() {
    currentState->update();
}

void Game::render() {
    currentState->render(window);
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}