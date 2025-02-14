#pragma once
#include "GameEvent.hpp"

/**
 * @brief Game lifecycle events
 */
class GameLifecycleEvent : public GameEvent {
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