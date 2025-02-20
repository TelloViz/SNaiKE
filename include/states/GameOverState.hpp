#pragma once
#include "State.hpp"
#include "AI/AIPlayer.hpp"  // Add this include for AIStrategy
#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>

class GameController;
class StateMachine;

class GameOverState : public State {
private:
    GameController* controller;
    StateContext context;
    StateMachine* machine;
    sf::Text gameOverText;
    sf::Text scoreText;
    int finalScore;

public:
    GameOverState(GameController* ctrl, const StateContext& ctx, StateMachine* mach, int score = 0);
    void handleInput(const GameInput& input) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void freeze() override;
    void unfreeze() override;
    std::string getStateName() const override { return "GameOverState"; }
};