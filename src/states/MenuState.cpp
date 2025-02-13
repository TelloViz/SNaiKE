#include "Game.hpp"
#include "states/MenuState.hpp"


MenuState::MenuState(Game* game, const StateContext& context) 
: State(game, context), selectedOption(0) {
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
                
            case sf::Keyboard::Enter:
                if (selectedOption == 0) {
                    game->changeState(std::make_unique<PlayingState>(game, context));
                } else if (selectedOption == 1) {
                    game->quit(); // We'll need to add this method
                }
                break;
        }
    }
}

void MenuState::update() {
    // Update menu animations if any
}

void MenuState::render(sf::RenderWindow& window) {
    // Just draw elements, no clear/display
    window.draw(titleText);
    for (const auto& option : menuOptions) {
        window.draw(option);
    }
}

void MenuState::pause() {
    // Menu state pause implementation (can be empty)
}

void MenuState::resume() {
    // Menu state resume implementation (can be empty)
}