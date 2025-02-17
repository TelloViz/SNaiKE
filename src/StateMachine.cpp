#include "StateMachine.hpp"
#include "State.hpp"

#include <iostream>  // Add this for console logging
#include <string>

void StateMachine::pushState(std::unique_ptr<State> state) {
    if (!state) {
        return;
    }
    
    isAdding = true;
    pendingState = std::move(state);
}

void StateMachine::popState() {
    if (!states.empty()) {
        std::string stateName = states.top()->getStateName();
        isRemoving = true;
        
        // If there's a state below, make sure to unfreeze it
        if (states.size() > 1) {
            auto it = states._Get_container().rbegin() + 1;  // Get second-to-top state
            (*it)->unfreeze();
        }
    }
}

void StateMachine::replaceState(std::unique_ptr<State> state) {
    isAdding = true;
    isReplacing = true;
    pendingState = std::move(state);
}

void StateMachine::processStateChanges() {
    if (isRemoving) {
        if (!states.empty()) {
            states.pop();
        }
        isRemoving = false;
    }

    if (isAdding) {
        if (!states.empty() && !isReplacing) {
            states.top()->freeze();
        }

        if (isReplacing && !states.empty()) {
            states.pop();
            isReplacing = false;
        }

        states.push(std::move(pendingState));
        isAdding = false;
    }
}