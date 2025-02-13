#pragma once
#include "../State.hpp"
#include <vector>

class GameController;
class StateMachine;

class MenuState : public State {
private:
    sf::Text titleText;
    std::vector<sf::Text> menuOptions;
    int selectedOption;

public:
    MenuState(GameController* controller, const StateContext& context, StateMachine* machine);
    void handleInput(const sf::Event& event) override;
    void update() override;
    void render(sf::RenderWindow& window) override;
    void pause() override;
    void resume() override;
};