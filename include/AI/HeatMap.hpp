#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <limits>
#include "GameConfig.hpp"

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
        for (int x = 0; x < GameConfig::GRID_WIDTH; x++) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; y++) {
                float normalizedValue = (scores[x][y] - minValue) / (maxValue - minValue);
                
                sf::RectangleShape cell(sf::Vector2f(GameConfig::CELL_SIZE, GameConfig::CELL_SIZE));
                cell.setPosition(
                    x * GameConfig::CELL_SIZE + GameConfig::MARGIN_SIDES,
                    y * GameConfig::CELL_SIZE + GameConfig::MARGIN_TOP
                );
                
                // Create a heat color (red for high values, blue for low)
                sf::Color cellColor(
                    static_cast<sf::Uint8>(255 * normalizedValue),  // Red
                    0,                                              // Green
                    static_cast<sf::Uint8>(255 * (1-normalizedValue)) // Blue
                );
                cellColor.a = 128;  // 50% transparency
                
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