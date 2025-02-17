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

void PausedState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Escape: {
                if (machine) {
                    machine->popState();
                } else {
                }
                break;
            }
            case sf::Keyboard::L:
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