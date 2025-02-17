#pragma once

/**
 * @brief Static configuration class containing game-wide constants
 * 
 * Centralizes all game configuration values in one place for easy modification.
 * All members are static constexpr to ensure compile-time optimization.
 */
struct GameConfig {
    // Gameplay area dimensions (where snake moves)
    static const int GRID_WIDTH = 20;
    static const int GRID_HEIGHT = 20;
    static const int CELL_SIZE = 20;
    
    // UI margins and border
    static const int MARGIN_TOP = 40;     // Space for score at top
    static const int MARGIN_BOTTOM = 40;  // Space for FPS/debug info
    static const int MARGIN_SIDES = 20;   // Side margins
    static const int BORDER_THICKNESS = 2; // Border around play area
    
    // Calculate total window dimensions
    static const int WINDOW_WIDTH = (GRID_WIDTH * CELL_SIZE) + (MARGIN_SIDES * 2);
    static const int WINDOW_HEIGHT = (GRID_HEIGHT * CELL_SIZE) + MARGIN_TOP + MARGIN_BOTTOM;
    
    static const int FRAME_RATE = 60;
};