#include "StateMachine.hpp"

void StateMachine::addState(std::unique_ptr<State> state, bool replace) {
    isAdding = true;
    isReplacing = replace;
    pendingState = std::move(state);
}

void StateMachine::removeState() {
    isRemoving = true;
}

void StateMachine::processStateChanges() {
    if (isRemoving && !states.empty()) {
        states.pop();
        isRemoving = false;
    }

    if (isAdding) {
        if (!states.empty() && isReplacing) {
            states.pop();
        }

        if (!states.empty()) {
            states.top()->pause();
        }

        states.push(std::move(pendingState));
        isAdding = false;
    }
}

std::unique_ptr<State>& StateMachine::getCurrentState() {
    if (states.empty()) {
        throw std::runtime_error("State stack is empty");
    }
    return states.top();
}