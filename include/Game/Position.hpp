#pragma once
#include <SFML/System/Vector2.hpp>

struct Position {
    sf::Vector2i pos;

    // Constructors
    Position() : pos(0, 0) {}
    Position(const sf::Vector2i& vector) : pos(vector) {}
    Position(int x, int y) : pos(x, y) {}

    // Comparison operators
    bool operator<(const Position& other) const {
        return pos.y < other.pos.y || (pos.y == other.pos.y && pos.x < other.pos.x);
    }
    
    bool operator==(const Position& other) const {
        return pos.x == other.pos.x && pos.y == other.pos.y;
    }

    bool operator!=(const Position& other) const {
        return !(*this == other);
    }

    // Conversion operator
    operator sf::Vector2i() const { return pos; }
};