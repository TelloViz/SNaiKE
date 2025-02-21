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

    void render(sf::RenderTarget& target, const sf::Vector2f& cellSize, float opacity = 0.85f) const {
        std::lock_guard<std::mutex> lock(scoreMutex);
        static sf::RectangleShape cell(cellSize);
        
        // First pass: Draw danger and safety zones
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (std::abs(score) < 0.1f || score >= 500.0f) continue; // Skip path and markers

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                if (score > 0) {
                    // Safe spaces - Magenta to white gradient
                    float normalized = std::min(1.0f, score / 300.0f);
                    cell.setFillColor(sf::Color(
                        static_cast<sf::Uint8>(180 + 75 * normalized),  // R (180-255)
                        static_cast<sf::Uint8>(50 + 205 * normalized),  // G (50-255)
                        static_cast<sf::Uint8>(180 + 75 * normalized),  // B (180-255)
                        static_cast<sf::Uint8>(220 * opacity)           // More opaque
                    ));
                } else {
                    // Danger zones - Deep red to bright red gradient
                    float normalized = std::min(1.0f, -score / 500.0f);
                    cell.setFillColor(sf::Color(
                        static_cast<sf::Uint8>(180 + 75 * normalized),  // R (180-255)
                        0,                                              // G
                        static_cast<sf::Uint8>(50 * normalized),        // B (0-50)
                        static_cast<sf::Uint8>(230 * opacity)           // Very opaque
                    ));
                }
                target.draw(cell);
            }
        }

        // Second pass: Draw planned path with bright cyan
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (score < 500.0f || score >= 900.0f) continue; // Only draw path

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                // Planned path - Bright electric blue
                float intensity = std::min(1.0f, (score - 500.0f) / 300.0f);
                cell.setFillColor(sf::Color(
                    static_cast<sf::Uint8>(50 * intensity),   // R (slight blue tint)
                    static_cast<sf::Uint8>(200 * intensity),  // G
                    255,                                      // B (always full)
                    255                                       // Fully opaque
                ));
                target.draw(cell);
            }
        }

        // Final pass: Draw critical positions
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (score < 900.0f) continue; // Only draw markers

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                // Critical positions - Bright gold
                cell.setFillColor(sf::Color(255, 215, 0, 255));  // More golden yellow
                target.draw(cell);
            }
        }
    }

    void triggerUpdate() {
        needsUpdate = true;
    }
};