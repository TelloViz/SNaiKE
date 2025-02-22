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
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up:    
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Up}; 
                break;
            case sf::Keyboard::Down:  
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Down}; 
                break;
            case sf::Keyboard::Left:  
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Left}; 
                break;
            case sf::Keyboard::Right: 
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Right}; 
                break;
            case sf::Keyboard::Return:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Select}; 
                break;
            case sf::Keyboard::Escape:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Back}; 
                break;
            case sf::Keyboard::T:     
                currentInput = GameInput{InputType::ButtonPressed, GameButton::ToggleAI}; 
                break;
            case sf::Keyboard::Num1:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num1};
                break;
            case sf::Keyboard::Num2:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num2};
                break;
            case sf::Keyboard::Num3:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num3};
                break;
            case sf::Keyboard::Num4:  // Add this
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num4};
                break;
            case sf::Keyboard::Numpad1:  // Also support numpad
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num1};
                break;
            case sf::Keyboard::Numpad2:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num2};
                break;
            case sf::Keyboard::Numpad3:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::Num3};
                break;
            case sf::Keyboard::H:
                currentInput = GameInput{InputType::ButtonPressed, GameButton::H};
                break;
            case sf::Keyboard::J:  // Add J key handling
                currentInput = GameInput{InputType::ButtonPressed, GameButton::J};
                break;
            default: 
                currentInput = GameInput{InputType::ButtonPressed, GameButton::None}; 
                break;
        }

        if (currentInput.type != InputType::ButtonPressed || 
            currentInput.button != GameButton::None) {
            inputQueue.push(currentInput);
        }
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
            case sf::Keyboard::T:     return GameInput{InputType::ButtonPressed, GameButton::ToggleAI};
            case sf::Keyboard::H:     return GameInput{InputType::ButtonPressed, GameButton::H};
            case sf::Keyboard::J:     return GameInput{InputType::ButtonPressed, GameButton::J}; // Add J key translation
            case sf::Keyboard::Num1:  return GameInput{InputType::ButtonPressed, GameButton::Num1};
            case sf::Keyboard::Num2:  return GameInput{InputType::ButtonPressed, GameButton::Num2};
            case sf::Keyboard::Num3:  return GameInput{InputType::ButtonPressed, GameButton::Num3};
            case sf::Keyboard::Num4:  return GameInput{InputType::ButtonPressed, GameButton::Num4};  // Add this
            default: return GameInput{InputType::ButtonPressed, GameButton::None};
        }
    }
    return GameInput{InputType::ButtonPressed, GameButton::None};
}