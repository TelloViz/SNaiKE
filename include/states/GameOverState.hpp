#pragma once
#include "../State.hpp"

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