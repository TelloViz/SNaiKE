#pragma once
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <vector>
#include "GameEvent.hpp"

/**
 * @brief Manages event subscriptions and dispatching
 * 
 * The EventManager provides a type-safe event system that:
 * - Allows registration of event handlers
 * - Dispatches events to registered handlers
 * - Supports multiple handlers per event type
 * - Uses static typing to ensure type safety
 */
class EventManager {
private:
    /** Function type for event handlers that can handle any GameEvent */
    using HandlerFn = std::function<void(const GameEvent&)>;

    /** 
     * Map of event types to their handlers
     * Key: Type information of the event
     * Value: Vector of handlers that can process that event type
     */
    std::unordered_map<std::type_index, std::vector<HandlerFn>> handlers;

public:
    /**
     * @brief Subscribes a handler function to an event type
     * @tparam T The event type to subscribe to (must derive from GameEvent)
     * @param handler Function that will handle events of type T
     * 
     * Example:
     * @code
     * eventManager.subscribe<GameLifecycleEvent>([](const GameLifecycleEvent& e) {
     *     if (e.type == GameLifecycleEvent::Type::GameOver) {
     *         // Handle game over...
     *     }
     * });
     * @endcode
     */
    template<typename T>
    void subscribe(std::function<void(const T&)> handler) {
        handlers[typeid(T)].push_back(
            [handler](const GameEvent& e) {
                handler(static_cast<const T&>(e));
            }
        );
    }

    /**
     * @brief Publishes an event to all registered handlers
     * @param event The event to publish (must derive from GameEvent)
     * 
     * Example:
     * @code
     * eventManager.publish(GameLifecycleEvent(GameLifecycleEvent::Type::GameOver));
     * @endcode
     */
    void publish(const GameEvent& event) {
        auto it = handlers.find(typeid(event));
        if (it != handlers.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }
};