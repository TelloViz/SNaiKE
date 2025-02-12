#include "Game.hpp"
#include <random>

Game::Game() 
    : window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Snake Game")
    , food(15, 15)
    , gameOver(false) {
    window.setFramerateLimit(10);
}

void Game::spawnFood() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> disY(0, GRID_HEIGHT - 1);
    food = sf::Vector2i(disX(gen), disY(gen));
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Up: snake.setDirection(Direction::Up); break;
                case sf::Keyboard::Down: snake.setDirection(Direction::Down); break;
                case sf::Keyboard::Left: snake.setDirection(Direction::Left); break;
                case sf::Keyboard::Right: snake.setDirection(Direction::Right); break;
            }
        }
    }
}

void Game::update() {
    if (gameOver) return;
    
    snake.move();
    
    if (snake.checkCollision(GRID_WIDTH, GRID_HEIGHT)) {
        gameOver = true;
        return;
    }

    if (snake.eat(food)) {
        spawnFood();
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    // Draw food
    sf::RectangleShape foodShape(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
    foodShape.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
    foodShape.setFillColor(sf::Color::Red);
    window.draw(foodShape);
    
    snake.draw(window, CELL_SIZE);
    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        processEvents();
        update();
        render();
    }
}