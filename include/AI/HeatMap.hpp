#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <limits>
#include "GameConfig.hpp"

class HeatMap {
private:
    std::vector<std::vector<float>> scores;
    float maxValue{-std::numeric_limits<float>::max()};
    float minValue{std::numeric_limits<float>::max()};

public:
    HeatMap() {
        clear();
    }

    void clear() {
        scores.assign(GameConfig::GRID_WIDTH, 
            std::vector<float>(GameConfig::GRID_HEIGHT, 0.0f));
        maxValue = -std::numeric_limits<float>::max();
        minValue = std::numeric_limits<float>::max();
    }

    void setValue(int x, int y, float value) {
        if (x >= 0 && x < GameConfig::GRID_WIDTH && 
            y >= 0 && y < GameConfig::GRID_HEIGHT) {
            scores[x][y] = value;
            if (value != 0) {
                maxValue = std::max(maxValue, value);
                minValue = std::min(minValue, value);
            }
        }
    }

    void render(sf::RenderTarget& target, const sf::Vector2f& cellSize) const {
        sf::RectangleShape cell(cellSize);
        
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                if (scores[x][y] != 0) {
                    float normalizedScore = (maxValue != minValue) ?
                        (scores[x][y] - minValue) / (maxValue - minValue) : 0.5f;
                    
                    // Enhanced color scheme
                    sf::Color color;
                    if (normalizedScore < 0.33f) {
                        // Dark blue to blue
                        color = sf::Color(
                            0,                                               // R
                            0,                                               // G
                            static_cast<sf::Uint8>(128 + 127 * normalizedScore * 3), // B
                            180                                              // A
                        );
                    } else if (normalizedScore < 0.66f) {
                        // Blue to yellow
                        float t = (normalizedScore - 0.33f) * 3;
                        color = sf::Color(
                            static_cast<sf::Uint8>(255 * t),     // R
                            static_cast<sf::Uint8>(255 * t),     // G
                            static_cast<sf::Uint8>(255 * (1-t)), // B
                            180                                   // A
                        );
                    } else {
                        // Yellow to red
                        float t = (normalizedScore - 0.66f) * 3;
                        color = sf::Color(
                            255,                                 // R
                            static_cast<sf::Uint8>(255 * (1-t)), // G
                            0,                                   // B
                            180                                  // A
                        );
                    }
                    
                    cell.setPosition(
                        x * cellSize.x + GameConfig::MARGIN_SIDES,
                        y * cellSize.y + GameConfig::MARGIN_TOP
                    );
                    cell.setFillColor(color);
                    target.draw(cell);
                }
            }
        }
    }
};