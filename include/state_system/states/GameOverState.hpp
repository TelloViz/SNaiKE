#pragma once

// Standard Library Includes
#include <vector>
#include <utility>

// Third-Party Includes
#include <SFML/Graphics.hpp>

// Project Includes
#include "state_system/State.hpp"
#include "ai/AIPlayer.hpp" 



namespace state_system::states {

class state_system::StateMachine;

class GameOverState : public state_system::State {
private:
    ::GameController* controller;
    state_system::StateContext context;
    state_system::StateMachine* machine;
    sf::Text gameOverText;
    sf::Text scoreText;
    int finalScore;

public:
    GameOverState(::GameController* ctrl, const state_system::StateContext& ctx, state_system::StateMachine* mach, int score = 0);
    void handleInput(const GameInput& input) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void freeze() override;
    void unfreeze() override;
    std::string getStateName() const override { return "GameOverState"; }
};

} // namespace state_system::states