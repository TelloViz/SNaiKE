#pragma once
#include <memory>
#include "State.hpp"
#include "StateTypes.hpp"

class GameController;
class GameResources;
class StateMachine;

class StateFactory {
public:
    static std::unique_ptr<State> createState(
        StateType type,
        GameController* controller,
        const GameResources& resources,
        StateMachine* machine
    );
};