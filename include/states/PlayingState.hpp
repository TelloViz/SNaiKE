#pragma once
#include "../State.hpp"
#include "../Snake.hpp"
#include <random>

class GameController;
class StateMachine;

class PlayingState : public State {
private:
    Snake snake;
    sf::Vector2i food;
    std::mt19937 rng;

    void spawnFood();

public:
    PlayingState(GameController* controller, const StateContext& context, StateMachine* machine);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void pause() override;
    void resume() override;
};