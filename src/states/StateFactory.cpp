#include "states/StateFactory.hpp"
#include "states/MenuState.hpp"
#include "states/PlayingState.hpp"
#include "states/PausedState.hpp"
#include "states/GameOverState.hpp"
#include "GameResources.hpp"

std::unique_ptr<State> StateFactory::createState(
    StateType type,
    GameController* controller,
    const GameResources& resources,
    StateMachine* machine
) {
    switch(type) {
        case StateType::Menu:
            return std::make_unique<MenuState>(controller, resources, machine);
        case StateType::Playing:
            return std::make_unique<PlayingState>(controller, resources, machine);
        case StateType::Paused:
            return std::make_unique<PausedState>(controller, resources, machine);
        case StateType::GameOver:
            return std::make_unique<GameOverState>(controller, resources, machine);
        default:
            throw std::runtime_error("Unknown state type");
    }
}