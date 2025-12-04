#pragma once
#include <ostream>

struct Vec2 {
    int x;
    int y;

    Vec2(int x = 0, int y = 0)
        : x(x), y(y) {
    }

    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(const Vec2& other) const {
        return Vec2(x * other.x, y * other.y);
    }

    bool operator<(const Vec2& other) const {
        if (x < other.x) {
            return true;
        }

        return x == other.x && y < other.y;
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;

        return *this;
    }
    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
};

Vec2 operator*(const int& scal, const Vec2& vec) {
    return Vec2(scal * vec.x, scal * vec.y);
}

std::ostream& operator<<(std::ostream& os, const Vec2& pos) {
    os << pos.x << "," << pos.y;
    return os;
}

#include <map>

enum class Direction {
    UP,
    LEFT,
    DOWN,
    RIGHT
};

Direction turnLeft(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 1) % 4);
}

Direction turnRight(Direction dir) {
    return static_cast<Direction>((static_cast<int>(dir) + 3) % 4);
}

Direction operator-(const Direction& other) {
    return static_cast<Direction>((static_cast<int>(other) + 2) % 4);
}

static const std::map<Direction, Vec2> directionVectors = {
    {Direction::UP, Vec2(0, -1)},
    {Direction::LEFT, Vec2(1, 0)},
    {Direction::DOWN, Vec2(0, 1)},
    {Direction::RIGHT, Vec2(-1, 0)},
};