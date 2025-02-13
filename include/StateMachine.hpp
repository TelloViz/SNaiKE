#pragma once
#include <memory>
#include <stack>
#include "State.hpp"

class StateMachine {
private:
    std::stack<std::unique_ptr<State>> states;
    std::unique_ptr<State> pendingState;
    bool isRemoving{false};
    bool isAdding{false};
    bool isReplacing{false};

public:
    void pushState(std::unique_ptr<State> state);
    void popState();
    void replaceState(std::unique_ptr<State> state);
    void processStateChanges();
    bool hasState() const { return !states.empty(); }
    State* getCurrentState() const;
};