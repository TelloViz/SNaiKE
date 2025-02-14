#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Shared context data for game states
 */
struct StateContext {
    const sf::Font& font;
    int width;
    int height;
    int cellSize;

    StateContext(const sf::Font& f, int w, int h, int cs)
        : font(f), width(w), height(h), cellSize(cs) {}
};