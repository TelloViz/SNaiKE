#pragma once
#include <memory>
#include <stack>
#include "State.hpp"

/**
 * @brief Manages game state transitions and stack
 * 
 * The StateMachine provides:
 * - Stack-based state management
 * - Safe state transitions (push, pop, replace)
 * - Deferred state changes through pending operations
 * - Access to current active state
 */
class StateMachine {
private:
    std::stack<std::unique_ptr<State>> states;     ///< Stack of active states
    std::unique_ptr<State> pendingState;           ///< State waiting to be added
    bool isRemoving{false};                        ///< Flag for pending pop operation
    bool isAdding{false};                          ///< Flag for pending push operation
    bool isReplacing{false};                       ///< Flag for pending replace operation

public:
    /**
     * @brief Pushes a new state onto the stack
     * @param state Unique pointer to state to add
     * 
     * State is not added immediately but queued for next update
     */
    void pushState(std::unique_ptr<State> state);

    /**
     * @brief Removes the top state from the stack
     * 
     * State is not removed immediately but queued for next update
     */
    void popState();

    /**
     * @brief Replaces current state with a new one
     * @param state Unique pointer to state that will replace current
     * 
     * State is not replaced immediately but queued for next update
     */
    void replaceState(std::unique_ptr<State> state);

    /**
     * @brief Processes any pending state changes
     * 
     * Called during update to handle:
     * - State additions
     * - State removals
     * - State replacements
     * Also manages proper pause/resume of affected states
     */
    void processStateChanges();

    /**
     * @brief Checks if state stack has any states
     * @return true if states exist, false if empty
     */
    bool hasState() const { return !states.empty(); }

    /**
     * @brief Gets pointer to current active state
     * @return Raw pointer to current state
     * @throws std::runtime_error if state stack is empty
     */
    State* getCurrentState() const;
};

// TODO (1): Integrate observer pattern for state changes
// TODO (2): Add state transition logging for debugging
// TODO (3): Implement state stack clearing method
// TODO (4): Add state stack size limit for safety
// TODO (5): Implement state stack serialization for saving
// TODO (6): Add state stack restore method for loading
// TODO (7): Integrate state stack with game save/load
