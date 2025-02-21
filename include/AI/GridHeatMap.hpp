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

    void render(sf::RenderTarget& target, const sf::Vector2f& cellSize, float opacity = 1.0f) const {
        // Add at start of render method
        std::cout << "\n=== GridHeatMap Render Start ===\n";
        std::cout << "Cells with scores > 1.0f:\n";
        int cellCount = 0;
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                if (scores[x][y] > 1.0f) {
                    cellCount++;
                }
            }
        }
        std::cout << "Total cells to render: " << cellCount << "\n";

        if (updateClock.getElapsedTime().asMilliseconds() < 100) {
            return;
        }
        updateClock.restart();

        std::lock_guard<std::mutex> lock(scoreMutex);
        static sf::RectangleShape cell(cellSize);
        static std::vector<std::pair<sf::Vector2f, sf::Color>> renderQueue;
        renderQueue.clear();
        
        // Debug print
        std::cout << "GridHeatMap rendering with min: " << minValue << " max: " << maxValue << "\n";
        
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                // Only process cells with significant positive values
                if (scores[x][y] > 1.0f) {
                    float normalizedScore = (maxValue != minValue) ? 
                        (scores[x][y] - minValue) / (maxValue - minValue) : 0.5f;

                    sf::Color color;
                    if (scores[x][y] > 150.0f) {
                        // Pure yellow for path and food
                        color = sf::Color(255, 255, 0, 255);
                        std::cout << "Yellow at " << x << "," << y << " score: " << scores[x][y] << "\n";
                    } else {
                        // Pure red-yellow gradient (no blue component at all)
                        float t = normalizedScore;
                        color = sf::Color(
                            255,                                    // R always full
                            static_cast<sf::Uint8>(255 * t),       // G varies from 0 to 255
                            0,                                      // B always zero
                            255                                     // A always full
                        );
                        std::cout << "Red-Yellow at " << x << "," << y << " score: " << scores[x][y] << "\n";
                    }

                    renderQueue.emplace_back(
                        sf::Vector2f(
                            x * cellSize.x + GameConfig::MARGIN_SIDES,
                            y * cellSize.y + GameConfig::MARGIN_TOP
                        ),
                        color
                    );
                }
            }
        }

        for (const auto& [pos, color] : renderQueue) {
            cell.setPosition(pos);
            cell.setFillColor(color);
            target.draw(cell);
        }
    }

    void triggerUpdate() {
        needsUpdate = true;
    }
};