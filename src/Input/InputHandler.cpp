#include "Input/InputHandler.hpp"

bool InputHandler::hasInput() const {
    return !inputQueue.empty();
}

GameInput InputHandler::getNextInput() {
    GameInput input = inputQueue.front();
    inputQueue.pop();
    return input;
}

void InputHandler::handleSFMLEvent(const sf::Event& event) {
    GameInput gameInput = translateSFMLEvent(event);
    if (gameInput.type != InputType::ButtonPressed || 
        gameInput.button != GameButton::None) {
        inputQueue.push(gameInput);
    }
}

GameInput InputHandler::translateSFMLEvent(const sf::Event& event) {
    if (event.type == sf::Event::Closed) {
        return GameInput{InputType::WindowClosed, GameButton::Quit};
    }
    else if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up:    return GameInput{InputType::ButtonPressed, GameButton::Up};
            case sf::Keyboard::Down:  return GameInput{InputType::ButtonPressed, GameButton::Down};
            case sf::Keyboard::Left:  return GameInput{InputType::ButtonPressed, GameButton::Left};
            case sf::Keyboard::Right: return GameInput{InputType::ButtonPressed, GameButton::Right};
            case sf::Keyboard::Return:return GameInput{InputType::ButtonPressed, GameButton::Select};
            case sf::Keyboard::Escape:return GameInput{InputType::ButtonPressed, GameButton::Back};
            default: return GameInput{InputType::ButtonPressed, GameButton::None};
        }
    }
    return GameInput{InputType::ButtonPressed, GameButton::None};
}