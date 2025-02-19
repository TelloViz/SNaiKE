#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <mutex>
#include <atomic>
#include "GameConfig.hpp"

class GridHeatMap {
private:
    std::vector<std::vector<float>> scores;
    mutable std::mutex scoreMutex;
    std::atomic<bool> needsUpdate{false};
    float maxValue{-std::numeric_limits<float>::max()};
    float minValue{std::numeric_limits<float>::max()};
    mutable sf::Clock updateClock;  // Make clock mutable to allow modification in const methods

public:
    GridHeatMap() {
        clear();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(scoreMutex);
        scores.assign(GameConfig::GRID_WIDTH, 
            std::vector<float>(GameConfig::GRID_HEIGHT, 0.0f));
        maxValue = -std::numeric_limits<float>::max();
        minValue = std::numeric_limits<float>::max();
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
        }
    }

    void render(sf::RenderTarget& target, const sf::Vector2f& cellSize, float opacity = 0.4f) const {
        // Only update visual every 100ms
        if (updateClock.getElapsedTime().asMilliseconds() < 100) {
            return;
        }
        updateClock.restart();

        std::lock_guard<std::mutex> lock(scoreMutex);
        static sf::RectangleShape cell(cellSize);
        
        // Pre-calculate positions and colors
        static std::vector<std::pair<sf::Vector2f, sf::Color>> renderQueue;
        renderQueue.clear();
        
        for (int x = 0; x < GameConfig::GRID_WIDTH; ++x) {
            for (int y = 0; y < GameConfig::GRID_HEIGHT; ++y) {
                if (std::abs(scores[x][y]) > 0.001f) {
                    float normalizedScore = (maxValue != minValue) ? 
                        (scores[x][y] - minValue) / (maxValue - minValue) : 0.5f;

                    sf::Color color(
                        static_cast<sf::Uint8>(255 * normalizedScore),
                        0,
                        static_cast<sf::Uint8>(255 * (1.0f - normalizedScore)),
                        static_cast<sf::Uint8>(255 * opacity)
                    );

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

        // Batch render
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