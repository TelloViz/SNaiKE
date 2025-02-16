#include "states/PausedState.hpp"
#include "GameController.hpp"
#include "debug/DebugLogger.hpp"
#include "debug/DebugOverlay.hpp"
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
                DebugLogger::log("Resuming from pause state");
                if (machine) {
                    machine->popState();
                    DebugLogger::log("Popped pause state");
                } else {
                    DebugLogger::log("Error: No state machine!");
                }
                break;
            }
            case sf::Keyboard::L:
                DebugLogger::toggleDebug();
                DebugLogger::log("Debug toggle pressed");
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