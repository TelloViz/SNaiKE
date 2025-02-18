#include "states/PausedState.hpp"
#include "GameController.hpp"

#include <iostream>

PausedState::PausedState(GameController* ctrl, const StateContext& ctx, StateMachine* mach)
    : State(ctrl, ctx, mach)
    , controller(ctrl)
    , context(ctx)
    , machine(mach) {
    
    pauseText.setFont(context.font);
    pauseText.setString("PAUSED\nPress ESC to resume");
    pauseText.setCharacterSize(30);
    pauseText.setFillColor(sf::Color::White);
    
    // Center the text
    sf::FloatRect textBounds = pauseText.getLocalBounds();
    pauseText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        (context.height * context.cellSize - textBounds.height) / 2
    );
}

void PausedState::handleInput(const GameInput& input) {
    if (input.type == InputType::ButtonPressed) {
        switch (input.button) {
            case GameButton::Back:
                std::cout << "PausedState: Back pressed, popping state" << std::endl;
                if (machine) {
                    machine->popState();
                    machine->processStateChanges();  // Important: Process the state change
                }
                break;
            default:
                break;
        }
    }
}

void PausedState::update() {
    // Paused state should not update the game
}

void PausedState::render(sf::RenderWindow& window) {
    // Render pause menu or overlay
    window.draw(pauseText);
}