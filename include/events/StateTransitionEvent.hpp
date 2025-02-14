#pragma once
#include "GameEvent.hpp"
#include <string>

/**
 * @brief Event triggered when state transitions should occur
 */
class StateTransitionEvent : public GameEvent {
public:
    enum class Type {
        Push,
        Pop,
        Replace
    };

    Type type;
    std::string targetState;  // State identifier instead of concrete class

    StateTransitionEvent(Type t, const std::string& state) 
        : type(t), targetState(state) {}
};