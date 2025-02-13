#pragma once
#include "../State.hpp"

class PausedState : public State {
private:
    sf::Text pausedText;

public:
    PausedState(GameController* controller, const StateContext& context, StateMachine* machine);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
};