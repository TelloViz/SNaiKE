#include "StateMachine.hpp"
#include "State.hpp"

//#include <iostream>  // Add this for console logging
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
    std::cout << "StateMachine::replaceState called" << std::endl;
    
    if (!state) {
        std::cerr << "Attempted to replace with null state" << std::endl;
        return;
    }

    isAdding = true;
    isReplacing = true;
    pendingState = std::move(state);
    
    std::cout << "State queued for replacement" << std::endl;
}

void StateMachine::processStateChanges() {
    std::cout << "Processing state changes... Current stack size: " << states.size() << std::endl;
    
    if (isRemoving && !states.empty()) {
        std::cout << "Removing state from stack size: " << states.size() << std::endl;
        states.pop();
        isRemoving = false;
        
        // Important: Unfreeze the state below when popping
        if (!states.empty()) {
            std::cout << "Unfreezing state below" << std::endl;
            states.top()->unfreeze();
        }
        std::cout << "After removal, stack size: " << states.size() << std::endl;
    }

    if (isAdding) {
        std::cout << "Adding new state with replacing=" << isReplacing << std::endl;
        
        if (!states.empty() && !isReplacing) {
            std::cout << "Freezing current state" << std::endl;
            states.top()->freeze();
        }

        if (isReplacing && !states.empty()) {
            std::cout << "Popping old state for replacement" << std::endl;
            states.pop();
        }

        if (pendingState) {
            std::cout << "Pushing new state" << std::endl;
            states.push(std::move(pendingState));
            std::cout << "New stack size: " << states.size() << std::endl;
        } else {
            std::cerr << "ERROR: Pending state is null!" << std::endl;
        }
        
        isAdding = false;
        isReplacing = false;
    }

    std::cout << "Final stack size: " << states.size() << std::endl;
}

// State* StateMachine::getCurrentState() const {
//     if (states.empty()) {
//         std::cout << "getCurrentState: Stack is empty!" << std::endl;
//         return nullptr;
//     }
//     return states.top().get();
// }