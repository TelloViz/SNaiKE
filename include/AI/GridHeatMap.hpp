#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <atomic>
#include "GameConfig.hpp"
#include <iostream>

class GridHeatMap {
private:
    std::vector<std::vector<float>> scores;
    mutable std::mutex scoreMutex;
    std::atomic<bool> needsUpdate{false};
    float maxValue{-std::numeric_limits<float>::max()};
    float minValue{std::numeric_limits<float>::max()};
    mutable sf::Clock updateClock;  // Make clock mutable to allow modification in const methods

public:
    GridHeatMap()
        : scores(GameConfig::GRID_WIDTH, std::vector<float>(GameConfig::GRID_HEIGHT, 0.0f))
        , needsUpdate(true) {}

    void clear() {
        std::lock_guard<std::mutex> lock(scoreMutex);
        for (auto& row : scores) {
            std::fill(row.begin(), row.end(), 0.0f);
        }
        maxValue = -std::numeric_limits<float>::max();
        minValue = std::numeric_limits<float>::max();
        needsUpdate = true;
    }

    void setValue(int x, int y, float value) {
        if (x >= 0 && x < GameConfig::GRID_WIDTH && 
            y >= 0 && y < GameConfig::GRID_HEIGHT) {
            std::lock_guard<std::mutex> lock(scoreMutex);
            scores[x][y] = value;
            if (value != 0) {
                maxValue = std::max(maxValue, value);
                minValue = std::min(minValue, value);
            }
            needsUpdate = true;
        }
    }

    float getValue(int x, int y) const {
        if (x >= 0 && x < GameConfig::GRID_WIDTH && y >= 0 && y < GameConfig::GRID_HEIGHT) {
            return scores[x][y];
        }
        return 0.0f;
    }

    void GridHeatMap::render(sf::RenderWindow& window, const sf::Vector2f& cellSize) const {
        std::lock_guard<std::mutex> lock(scoreMutex);
    
        sf::RectangleShape cell(cellSize);
        
        // Add margin offsets from GameConfig
        float xOffset = GameConfig::MARGIN_SIDES;
        float yOffset = GameConfig::MARGIN_TOP;
        
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (score == 0) continue;
    
                // Add offsets to position calculation
                cell.setPosition(
                    x * cellSize.x + xOffset, 
                    y * cellSize.y + yOffset
                );
                
                if (score >= 900.0f) {
                    cell.setFillColor(sf::Color(255, 255, 0, 255));
                }
                else if (score > 0) {
                    float alpha = std::min(255.0f, score / 3.0f);
                    cell.setFillColor(sf::Color(0, 0, 255, static_cast<sf::Uint8>(alpha)));
                }
                else {
                    float alpha = std::min(255.0f, -score / 2.0f);
                    cell.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(alpha)));
                }
                
                window.draw(cell);
            }
        }
    }
    void triggerUpdate() {
        needsUpdate = true;
    }
};