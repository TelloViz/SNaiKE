#pragma once

/**
 * @brief Static configuration class containing game-wide constants
 * 
 * Centralizes all game configuration values in one place for easy modification.
 * All members are static constexpr to ensure compile-time optimization.
 */
class GameConfig {
public:
    // Static game configuration
    static constexpr int CELL_SIZE = 20;
    static constexpr int GRID_WIDTH = 40;
    static constexpr int GRID_HEIGHT = 30;
    static constexpr int FRAME_RATE = 10;

    // Utility methods
    static constexpr int WindowWidth() { return GRID_WIDTH * CELL_SIZE; }
    static constexpr int WindowHeight() { return GRID_HEIGHT * CELL_SIZE; }
};