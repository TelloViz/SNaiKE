#include "StateMachine.hpp"
#include "State.hpp"
#include "debug/DebugOverlay.hpp"
#include "debug/DebugLogger.hpp"
#include <iostream>  // Add this for console logging
#include <string>

void StateMachine::pushState(std::unique_ptr<State> state) {
    DebugLogger::log("=== Push State Called ===");
    if (!state) {
        DebugLogger::log("ERROR: Attempting to push nullptr state");
        return;
    }
    
    DebugLogger::log("Pushing new state: " + state->getStateName());
    isAdding = true;
    pendingState = std::move(state);
    DebugLogger::log("State queued for next process cycle");
    DebugOverlay::getInstance().setValue("StateMachine", "State push pending");
}

void StateMachine::popState() {
    if (!states.empty()) {
        std::string stateName = states.top()->getStateName();
        DebugLogger::log("Popping state: " + stateName);
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
            DebugLogger::log("Processing state removal");
            states.pop();
            DebugLogger::log("State popped, remaining states: " + std::to_string(states.size()));
        }
        isRemoving = false;
    }

    if (isAdding) {
        DebugLogger::log("Processing state addition");
        if (!states.empty() && !isReplacing) {
            states.top()->freeze();
            DebugLogger::log("Froze current state");
        }

        if (isReplacing && !states.empty()) {
            DebugLogger::log("Replacing state");
            states.pop();
            isReplacing = false;
        }

        states.push(std::move(pendingState));
        DebugLogger::log("New state pushed");
        isAdding = false;
    }
}