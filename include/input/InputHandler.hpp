#pragma once
#include <SFML/Graphics.hpp>
#include <queue>

enum class GameButton {
    Up, Down, Left, Right,
    Select, Back, Start,
    Quit
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
    void pushEvent(const GameInput& input);
    bool hasInput() const;
    GameInput getNextInput();

private:
    std::queue<GameInput> inputQueue;
};

