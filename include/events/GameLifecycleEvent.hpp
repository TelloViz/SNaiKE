#pragma once
#include "Event.hpp"

/**
 * @brief Game lifecycle events
 */
class GameLifecycleEvent : public Event {
public:
    enum class Type {
        GameStart,
        GameOver,
        GamePause,
        GameResume,
        GameQuit
    };

    Type type;
    
    explicit GameLifecycleEvent(Type t) : type(t) {}
};