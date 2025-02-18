#pragma once
#include <SFML/Graphics.hpp>
#include <queue>

enum class GameButton {
    Up, Down, Left, Right,
    Select, Back, Start,
    Quit, None
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
public:
    void handleSFMLEvent(const sf::Event& event);
    bool hasInput() const;
    GameInput getNextInput();

private:
    std::queue<GameInput> inputQueue;
    GameInput translateSFMLEvent(const sf::Event& event);
};

