#include "GameLoop.hpp"
#include "states/MenuState.hpp"
#include "states/PlayingState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"

#include "GameConfig.hpp"

MenuState::MenuState(GameController* ctrl, const StateContext& ctx, StateMachine* mach) 
    : State(ctrl, ctx, mach)
    , selectedOption(0)
    , controller(ctrl)
    , context(ctx)
    , machine(mach) {
    // Title setup
    titleText.setFont(context.font);
    titleText.setString("SNAKE GAME");
    titleText.setCharacterSize(50);
    titleText.setFillColor(sf::Color::Green);
    
    // Center the title
    sf::FloatRect textBounds = titleText.getLocalBounds();
    titleText.setPosition(
        (context.width * context.cellSize - textBounds.width) / 2,
        context.height * context.cellSize * 0.2f
    );

    // Menu options setup
    std::vector<std::string> options = {"Play", "Quit"};
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text option;
        option.setFont(context.font);
        option.setString(options[i]);
        option.setCharacterSize(30);
        option.setFillColor(sf::Color::White);
        
        // Center each option
        textBounds = option.getLocalBounds();
        option.setPosition(
            (context.width * context.cellSize - textBounds.width) / 2,
            context.height * context.cellSize * (0.4f + i * 0.1f)
        );
        
        menuOptions.push_back(option);
    }
    
    // Highlight initial selection
    menuOptions[selectedOption].setFillColor(sf::Color::Green);

    menuText.setFont(context.font);
    menuText.setString("Press ENTER to Start\nPress ESC to Exit");
    menuText.setCharacterSize(24);
    menuText.setFillColor(sf::Color::White);
    
    // Center the text
    textBounds = menuText.getLocalBounds();
    menuText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
    menuText.setPosition(
        context.width * GameConfig::CELL_SIZE / 2.0f,
        context.height * GameConfig::CELL_SIZE / 2.0f
    );
    
}

void MenuState::handleInput(const GameInput& input) {
    if (input.type == InputType::ButtonPressed) {
        switch (input.button) {
            case GameButton::Up:
                menuOptions[selectedOption].setFillColor(sf::Color::White);
                selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
                menuOptions[selectedOption].setFillColor(sf::Color::Green);
                break;
                
            case GameButton::Down:
                menuOptions[selectedOption].setFillColor(sf::Color::White);
                selectedOption = (selectedOption + 1) % menuOptions.size();
                menuOptions[selectedOption].setFillColor(sf::Color::Green);
                break;
                
            case GameButton::Select:
                if (selectedOption == 0) {
                    handlePlaySelected();
                } else if (selectedOption == 1) {
                    controller->quitGame();
                }
                break;
        }
    }
}

void MenuState::handlePlaySelected() {
    try {
        auto playingState = std::make_unique<PlayingState>(controller, context, machine);
        if (!playingState) {
            throw std::runtime_error("Failed to create PlayingState");
        }
        
        machine->replaceState(std::move(playingState));
    } catch (const std::exception& e) {
    }
}

void MenuState::update() {
    // Update menu animations if any
}

void MenuState::render(sf::RenderWindow& window) {
    
    // Draw title
    window.draw(titleText);
    
    // Draw all menu options
    for (const auto& option : menuOptions) {
        window.draw(option);
    }
    

}

void MenuState::freeze() {
    // Menu state freeze implementation (can be empty)
}

void MenuState::unfreeze() {
    // Menu state unfreeze implementation (can be empty)
}