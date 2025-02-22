#pragma once
#include <SFML/Graphics.hpp>
#include <queue>

enum class GameButton {
    Up, Down, Left, Right,
    Select, Back, Start,
    Quit, ToggleAI, 
    Num1, Num2, Num3, Num4, H, G,
    None  // Added ToggleAI
};

enum class InputType {
    ButtonPressed,
    ButtonReleased,
    WindowClosed
};

struct GameInput {
    InputType type;
    GameButton button;
};

class InputHandler {
private:
    std::queue<GameInput> inputQueue;
    GameInput currentInput{InputType::ButtonPressed, GameButton::None};  // Add this line

public:
    void handleSFMLEvent(const sf::Event& event);
    bool hasInput() const;
    GameInput getNextInput();
    GameInput translateSFMLEvent(const sf::Event& event);
};

