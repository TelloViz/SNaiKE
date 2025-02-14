#pragma once

/**
 * @brief Aggregator header for all game states
 * 
 * Provides convenient single-include access to all game states:
 * - MenuState: Main menu and game entry point
 * - PlayingState: Active gameplay state
 * - PausedState: Game pause overlay
 * - GameOverState: End game state
 * 
 * Usage:
 * @code
 * #include "states/States.hpp"
 * @endcode
 */

#include "MenuState.hpp"
#include "PlayingState.hpp"
#include "PausedState.hpp"
#include "GameOverState.hpp"