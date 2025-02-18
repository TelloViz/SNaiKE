#pragma once
#include <SFML/System/Clock.hpp>

class GameClock {
private:
    sf::Clock clock;
    sf::Time pausedTime;
    bool isPaused;

public:
    GameClock() : pausedTime(sf::Time::Zero), isPaused(false) {}
    
    void restart() {
        clock.restart();
        pausedTime = sf::Time::Zero;
        isPaused = false;
    }
    
    sf::Time getElapsedTime() const {
        return isPaused ? pausedTime : clock.getElapsedTime();
    }
    
    void freeze() {
        if (!isPaused) {
            pausedTime = clock.getElapsedTime();
            isPaused = true;
        }
    }
    
    void unfreeze() {
        if (isPaused) {
            clock.restart();
            isPaused = false;
        }
    }
};