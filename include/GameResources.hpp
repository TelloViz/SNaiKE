#pragma once
#include <SFML/Graphics.hpp>

/**
 * @brief Container for runtime game resources
 * 
 * Manages runtime-loaded resources like:
 * - Fonts
 * - Textures
 * - Sound effects
 * - Music
 */
struct GameResources {
    const sf::Font& font;
    // Future: Add textures, sounds, etc.

    explicit GameResources(const sf::Font& gameFont)
        : font(gameFont) {}
};