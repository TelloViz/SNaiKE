#include "Game.hpp"
#include "GameStates.hpp"
#include <random>
#include <iostream>
#include <filesystem>


Game::Game() 
    : window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Snake Game") {
    window.setFramerateLimit(10);
    
    StateContext context{
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