#pragma once

/**
 * @brief Base class for all game events
 * 
 * Abstract base class that enforces proper event inheritance.
 * Private constructor prevents instantiation of bare GameEvents.
 */
class GameEvent {
public:
    virtual ~GameEvent() = default;

protected:
    // Allow derived classes to construct
    GameEvent() = default;

private:
    // Prevent direct instantiation
    friend class EventManager; // Optional: if EventManager needs direct access
};
