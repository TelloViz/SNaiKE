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
        
        // First pass: Distance gradient (blue)
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (std::abs(score) < 0.1f || score >= 600.0f) continue;

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                // Blue gradient for general scoring
                float normalized = std::min(1.0f, score / 100.0f);
                cell.setFillColor(sf::Color(
                    0,                                              // R
                    static_cast<sf::Uint8>(120 + 135 * normalized), // G
                    static_cast<sf::Uint8>(180 + 75 * normalized),  // B
                    static_cast<sf::Uint8>(230 * opacity)           // A
                ));
                target.draw(cell);
            }
        }

        // Second pass: Exploration paths (magenta)
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (score < 600.0f || score >= 800.0f) continue;  // 600-799 for exploration paths

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                // Magenta for exploration paths
                float intensity = (score - 600.0f) / 200.0f;
                cell.setFillColor(sf::Color(
                    255,                                            // R
                    static_cast<sf::Uint8>(50 + 50 * intensity),   // G
                    255,                                           // B
                    static_cast<sf::Uint8>(255 * opacity)          // A
                ));
                target.draw(cell);
            }
        }

        // Third pass: Current planned path (orange)
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (score < 800.0f || score >= 900.0f) continue;  // 800-899 for planned path

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                // Orange for planned path
                float intensity = (score - 800.0f) / 100.0f;
                cell.setFillColor(sf::Color(
                    255,                                            // R
                    static_cast<sf::Uint8>(140 + 69 * intensity),  // G
                    0,                                             // B
                    static_cast<sf::Uint8>(255 * opacity)          // A
                ));
                target.draw(cell);
            }
        }

        // Final pass: Critical positions (red)
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                float score = scores[x][y];
                if (score < 900.0f) continue;

                cell.setPosition(
                    x * cellSize.x + GameConfig::MARGIN_SIDES,
                    y * cellSize.y + GameConfig::MARGIN_TOP
                );

                // Red for food and head
                cell.setFillColor(sf::Color(255, 0, 0, static_cast<sf::Uint8>(255 * opacity)));
                target.draw(cell);
            }
        }
    }

    void triggerUpdate() {
        needsUpdate = true;
    }
};