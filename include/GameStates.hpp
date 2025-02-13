#pragma once
#include "State.hpp"
#include "Game.hpp"
#include "Snake.hpp"
#include <SFML/Graphics.hpp>
#include <random>   // For random number generation

class MenuState : public State {
private:
    sf::Text titleText;
    sf::Text startText;

public:
    explicit MenuState(Game* game, const StateContext& context);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
};

class PlayingState : public State {
private:
    Snake snake;  // Owned by PlayingState
    sf::Vector2i food;  // Owned by PlayingState
    std::mt19937 rng;  // Random number generator

    void spawnFood();

public:
    PlayingState(Game* game, const StateContext& context);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
};

class PausedState : public State {
private:
    sf::Text pausedText;

public:
    explicit PausedState(Game* game, const StateContext& context);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
};

class GameOverState : public State {
private:
    sf::Text gameOverText;
    sf::Text scoreText;

public:
    explicit GameOverState(Game* game, const StateContext& context);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
};