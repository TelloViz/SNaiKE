#pragma once
#include <SFML/System/Clock.hpp>

class StateClock {
private:
    sf::Clock clock;
    float frozenTimeElapsed{0.0f};
    bool wasFrozen{false};

public:
    void freeze() {
        frozenTimeElapsed = clock.getElapsedTime().asSeconds();
        wasFrozen = true;
    }

    void unfreeze() {
        if (wasFrozen) {
            clock.restart();
            wasFrozen = false;
        }
    }

    float getElapsedTime() const {
        return clock.getElapsedTime().asSeconds() + 
               (wasFrozen ? frozenTimeElapsed : 0.0f);
    }

    void restart() {
        clock.restart();
        frozenTimeElapsed = 0.0f;
        wasFrozen = false;
    }
};