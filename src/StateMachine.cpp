#include "StateMachine.hpp"

void StateMachine::pushState(std::unique_ptr<State> state) {
    isAdding = true;
    isReplacing = false;
    pendingState = std::move(state);
}

void StateMachine::popState() {
    isRemoving = true;
}

void StateMachine::replaceState(std::unique_ptr<State> state) {
    isAdding = true;
    isReplacing = true;
    pendingState = std::move(state);
}

void StateMachine::processStateChanges() {
    if (isRemoving && !states.empty()) {
        states.pop();
        if (!states.empty()) {
            states.top()->resume();  // Resume the previous state
        }
        isRemoving = false;
    }

    if (isAdding) {
        if (!states.empty() && isReplacing) {
            states.pop();
        }

        if (!states.empty()) {
            states.top()->pause();  // Pause the current state
        }

        states.push(std::move(pendingState));
        isAdding = false;
    }
}

State* StateMachine::getCurrentState() const {
    if (states.empty()) {
        throw std::runtime_error("State stack is empty");
    }
    return states.top().get();
}

void StateMachine::clearStates() {
    while (!states.empty()) {
        states.pop();
    }
    // Reset pending state and flags
    pendingState.reset();
    isRemoving = false;
    isAdding = false;
    isReplacing = false;
}