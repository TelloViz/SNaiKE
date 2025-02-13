#pragma once
#include <memory>
#include <stack>
#include "State.hpp"

class StateMachine {
private:
    std::stack<std::unique_ptr<State>> states;
    std::unique_ptr<State> pendingState;
    bool isRemoving;
    bool isAdding;
    bool isReplacing;

public:
    StateMachine() : isRemoving(false), isAdding(false), isReplacing(false) {}

    void addState(std::unique_ptr<State> state, bool replace = true);
    void removeState();
    void processStateChanges();
    bool hasState() const { return !states.empty(); }
    std::unique_ptr<State>& getCurrentState();
};