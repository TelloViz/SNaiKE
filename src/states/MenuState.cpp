#include "Game.hpp"
#include "states/MenuState.hpp"
#include "states/PlayingState.hpp"
#include "GameController.hpp"
#include "StateMachine.hpp"
#include "debug/DebugOverlay.hpp"
#include "debug/DebugLogger.hpp"
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
    
    DebugOverlay::getInstance().setValue("MenuState", "Initialized");
    DebugOverlay::getInstance().setValue("Font Loaded", context.font.getInfo().family);
}

void MenuState::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up:
                menuOptions[selectedOption].setFillColor(sf::Color::White);
                selectedOption = (selectedOption - 1 + menuOptions.size()) % menuOptions.size();
                menuOptions[selectedOption].setFillColor(sf::Color::Green);
                break;
                
            case sf::Keyboard::Down:
                menuOptions[selectedOption].setFillColor(sf::Color::White);
                selectedOption = (selectedOption + 1) % menuOptions.size();
                menuOptions[selectedOption].setFillColor(sf::Color::Green);
                break;
                
            case sf::Keyboard::Return:
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
    DebugLogger::log("=== Handling Play Selection ===");
    try {
        auto playingState = std::make_unique<PlayingState>(controller, context, machine);
        if (!playingState) {
            throw std::runtime_error("Failed to create PlayingState");
        }
        DebugLogger::log("Created PlayingState successfully");
        
        machine->replaceState(std::move(playingState));
        DebugLogger::log("Requested state replacement");
    } catch (const std::exception& e) {
        DebugLogger::log("Exception creating PlayingState: " + std::string(e.what()));
        DebugOverlay::getInstance().setValue("Error", "Failed to create PlayingState");
    }
}

void MenuState::update() {
    // Update menu animations if any
}

void MenuState::render(sf::RenderWindow& window) {
    DebugOverlay::getInstance().setValue("MenuState", "Rendering menu...");
    
    // Draw title
    window.draw(titleText);
    
    // Draw all menu options
    for (const auto& option : menuOptions) {
        window.draw(option);
    }
    
    // Debug info
    DebugOverlay::getInstance().setValue("Title Pos", 
        "x: " + std::to_string(titleText.getPosition().x) + 
        " y: " + std::to_string(titleText.getPosition().y));
    
    DebugOverlay::getInstance().setValue("Selected Option", 
        std::to_string(selectedOption));
}

void MenuState::freeze() {
    // Menu state freeze implementation (can be empty)
}

void MenuState::unfreeze() {
    // Menu state unfreeze implementation (can be empty)
}