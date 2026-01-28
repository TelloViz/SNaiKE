#pragma once

// Standard Library Includes
#include <vector>

// Third-Party Includes
#include <SFML/Graphics.hpp>

// Project Includes
#include "ai/BaseStrategy.hpp"
#include "Snake.hpp"

class HamiltonStrategy : public BaseStrategy {
private:
    const Snake& snake;
    std::vector<sf::Vector2i> hamiltonCycle;  // Stores the complete cycle
    size_t currentIndex;  // Current position in cycle
    bool cycleInitialized;
    bool showPathArrows = false;  

    // Helper methods
    void initializeHamiltonCycle();
    bool isValidNextPosition(const sf::Vector2i& pos) const;
    size_t findSnakeHeadInCycle() const;

public:
    HamiltonStrategy(const Snake& snakeRef);
    Direction calculateNextMove(const Snake& snake, const sf::Vector2i& food) override;
    void update() override;
    void render(sf::RenderWindow& window) const override;
    void toggleHeatMap() override;
    void togglePathArrows() { showPathArrows = !showPathArrows; }
    bool isPathArrowsEnabled() const { return showPathArrows; }
};