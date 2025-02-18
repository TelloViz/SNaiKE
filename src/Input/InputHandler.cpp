#include "Input/InputHandler.hpp"

void InputHandler::pushEvent(const GameInput& input) {
    inputQueue.push(input);
}

bool InputHandler::hasInput() const {
    return !inputQueue.empty();
}

GameInput InputHandler::getNextInput() {
    GameInput input = inputQueue.front();
    inputQueue.pop();
    return input;
}