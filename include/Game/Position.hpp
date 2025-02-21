#pragma once
#include <SFML/System/Vector2.hpp>

struct Position {
    sf::Vector2i pos{0, 0};  // Initialize with default values
    
    Position() : pos(0, 0) {}
    Position(const sf::Vector2i& p) : pos(p) {}
    Position(int x, int y) : pos(x, y) {}
    
    bool operator<(const Position& other) const {
        return pos.x < other.pos.x || (pos.x == other.pos.x && pos.y < other.pos.y);
    }
    
    bool operator==(const Position& other) const {
        return pos.x == other.pos.x && pos.y == other.pos.y;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
    
    operator sf::Vector2i() const { return pos; }
};