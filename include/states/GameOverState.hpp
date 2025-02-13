#pragma once
#include "../State.hpp"

class GameController;
class StateMachine;

class GameOverState : public State {
private:
    sf::Text gameOverText;
    sf::Text scoreText;

public:
    GameOverState(GameController* controller, const StateContext& context, StateMachine* machine);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void pause() override;
    void resume() override;
};