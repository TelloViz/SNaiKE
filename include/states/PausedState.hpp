#pragma once
#include "../State.hpp"

class PausedState : public State {
private:
    sf::Text pausedText;

public:
    explicit PausedState(Game* game, const StateContext& context);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
};