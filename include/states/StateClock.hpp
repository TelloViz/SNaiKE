#pragma once
#include <SFML/System/Clock.hpp>

class StateClock {
private:
    sf::Clock clock;
    float totalElapsedTime{0.0f};  // Track total time including pauses
    float timeAtFreeze{0.0f};      // Time when freeze was called
    bool wasFrozen{false};

public:
    void freeze() {
        if (!wasFrozen) {
            timeAtFreeze = clock.getElapsedTime().asSeconds();
            totalElapsedTime += timeAtFreeze;  // Add current time to total
            wasFrozen = true;
        }
    }

    void unfreeze() {
        if (wasFrozen) {
            clock.restart();  // Reset clock but keep total time
            wasFrozen = false;
        }
    }

    float getElapsedTime() const {
        if (wasFrozen) {
            return totalElapsedTime;  // Return cached total time while frozen
        }
        return totalElapsedTime + clock.getElapsedTime().asSeconds();
    }

    void restart() {
        clock.restart();
        totalElapsedTime = 0.0f;
        timeAtFreeze = 0.0f;
        wasFrozen = false;
    }
};