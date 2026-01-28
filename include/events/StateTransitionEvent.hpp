#pragma once

// Standard Library Includes
#include <string>

// Project Includes
#include "Event.hpp"


/**
 * @brief Event triggered when state transitions should occur
 */
class StateTransitionEvent : public Event {
public:
    enum class Type {
        Push,
        Pop,
        Replace
    };

    Type type;
    std::string targetState;

    StateTransitionEvent(Type t, const std::string& state) 
        : type(t), targetState(state) {}
};