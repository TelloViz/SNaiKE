#pragma once

/**
 * @brief Static configuration class containing game-wide constants
 * 
 * Centralizes all game configuration values in one place for easy modification.
 * All members are static constexpr to ensure compile-time optimization.
 */
class GameConfig {
public:
    /// Size of each grid cell in pixels
    static constexpr int CELL_SIZE = 20;

    /// Number of cells in grid width
    static constexpr int GRID_WIDTH = 40;

    /// Number of cells in grid height
    static constexpr int GRID_HEIGHT = 30;

    /// Target frame rate for game update loop
    static constexpr int FRAME_RATE = 10;

    /**
     * @brief Calculate total window width in pixels
     * @return Width in pixels (GRID_WIDTH * CELL_SIZE)
     */
    static constexpr int WindowWidth() { return GRID_WIDTH * CELL_SIZE; }

    /**
     * @brief Calculate total window height in pixels
     * @return Height in pixels (GRID_HEIGHT * CELL_SIZE)
     */
    static constexpr int WindowHeight() { return GRID_HEIGHT * CELL_SIZE; }
};