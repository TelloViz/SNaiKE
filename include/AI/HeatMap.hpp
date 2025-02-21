#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <limits>
#include "GameConfig.hpp"
#include <iostream>

class HeatMap {
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

    float getValue(int x, int y) const {
        if (x >= 0 && x < GameConfig::GRID_WIDTH && 
            y >= 0 && y < GameConfig::GRID_HEIGHT) {
            return scores[x][y];
        }
        return 0.0f;
    }

    void normalize() {
        if (maxValue != minValue) {
            for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
                for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                    scores[x][y] = (scores[x][y] - minValue) / (maxValue - minValue);
                }
            }
            maxValue = 1.0f;
            minValue = 0.0f;
        }
    }

    void render(sf::RenderWindow& window) const {
        std::cout << "\n=== Base HeatMap Render Start ===\n";
        std::cout << "MinValue: " << minValue << " MaxValue: " << maxValue << "\n";

        for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
                float normalizedValue = (maxValue != minValue) ? 
                    (scores[x][y] - minValue) / (maxValue - minValue) : 0.0f;
                
                sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE, GameConfig::CELL_SIZE));
                cell.setPosition(
                    x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES,
                    y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP
                );
                
                // Enhanced color scheme with more vibrant blue-yellow-red gradient
                sf::Color cellColor;
                if (normalizedValue < 0.33f) {
                    // Dark blue to bright blue
                    cellColor = sf::Color(
                        0,                                            // R
                        static_cast<sf::Uint8>(normalizedValue * 3 * 255),  // G
                        255,                                          // B
                        255                                           // A
                    );
                } else if (normalizedValue < 0.66f) {
                    // Bright blue to bright yellow
                    float t = (normalizedValue - 0.33f) * 3;
                    cellColor = sf::Color(
                        static_cast<sf::Uint8>(t * 255),      // R
                        255,                                   // G
                        static_cast<sf::Uint8>((1-t) * 255),  // B
                        255                                    // A
                    );
                } else {
                    // Bright yellow to bright red
                    float t = (normalizedValue - 0.66f) * 3;
                    cellColor = sf::Color(
                        255,                                   // R
                        static_cast<sf::Uint8>((1-t) * 255),  // G
                        0,                                     // B
                        255                                    // A
                    );
                }
                
                cell.setFillColor(cellColor);
                window.draw(cell);
            }
        }
    }
    
private:
    std::vector<std::vector<float>> scores;
    float maxValue{-std::numeric_limits<float>::max()};
    float minValue{std::numeric_limits<float>::max()};
};