#pragma once
#include "State.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include "input/InputHandler.hpp"


class MenuState : public State {
private:
    int selectedOption;
    sf::Text titleText;
    sf::Text menuText;  // Add menuText member
    std::vector<sf::Text> menuOptions;
    GameController* controller;
    StateContext context;
    StateMachine* machine;
    
    void handlePlaySelected();

public:
    MenuState(GameController* controller, const StateContext& context, StateMachine* machine);
    
    void update() override;
    void render(sf::RenderWindow& window) override;
    void handleInput(const GameInput& input) override;
    void freeze() override;    // Add override keyword
    void unfreeze() override;  // Add override keyword
    std::string getStateName() const override { return "MenuState"; }
};