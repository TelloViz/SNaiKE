#pragma once

/**
 * @brief Base class for all game events
 * 
 * Abstract base class that enforces proper event inheritance.
 * Private constructor prevents instantiation of bare GameEvents.
 */
class Event {
public:
    virtual ~Event() = default;

protected:
    // Allow derived classes to construct
    Event() = default;

private:
    // Prevent direct instantiation
    friend class EventManager; // Optional: if EventManager needs direct access
};
