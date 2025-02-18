#pragma once
#include "State.hpp"
#include <SFML/Graphics.hpp>

class GameController;
class StateMachine;

class GameOverState : public State {
private:
    GameController* controller;
    StateContext context;
    StateMachine* machine;
    sf::Text gameOverText;
    sf::Text scoreText;

public:
    GameOverState(GameController* ctrl, const StateContext& ctx, StateMachine* mach);
    void handleInput(const GameInput& input) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void freeze() override;
    void unfreeze() override;
    std::string getStateName() const override { return "GameOverState"; }
};