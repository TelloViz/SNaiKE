#pragma once
#include "state_system/State.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include "input/InputHandler.hpp"


namespace state_system::states {

class MenuState : public ::state_system::State {
private:
    int selectedOption;
    sf::Text titleText;
    sf::Text menuText; 
    std::vector<sf::Text> menuOptions;
    ::GameController* controller;
    state_system::StateContext context;
    state_system::StateMachine* machine;
    
    void handlePlaySelected();

public:
    MenuState(::GameController* controller, const state_system::StateContext& context, state_system::StateMachine* machine);
    
    void update() override;
    void render(sf::RenderWindow& window) override;
    void handleInput(const GameInput& input) override;
    void freeze() override;   
    void unfreeze() override; 
    std::string getStateName() const override { return "MenuState"; }
};

} // namespace state_system::states