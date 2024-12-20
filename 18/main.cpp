// #pragma once
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

// #include <Vec2.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <vector>

struct Map {
    char** map;
    int width, height;
    Vec2 start, end;
    int** scores;

    Map(int width, int height)
        : width(width), height(height) {
        map = new char*[width];
        scores = new int*[width];

        for (int x = 0; x < width; x++) {
            map[x] = new char[height];
            scores[x] = new int[height];

            for (int y = 0; y < height; y++) {
                map[x][y] = '.';
                scores[x][y] = INT_MAX;
            }
        }
    }

    const char& operator[](const Vec2& pos) const {
        return map[pos.x][pos.y];
    }

    char& operator[](const Vec2& pos) {
        return map[pos.x][pos.y];
    }

    void setScores(const Vec2& start, const Vec2& end, int currentScore) {
        if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) {
            return;
        }

        char& current = (*this)[start];
        if (current == '#') {
            return;
        }

        int score;
        int& minScore = scores[start.x][start.y];
        if (currentScore < minScore) {
            minScore = currentScore;
        }
        else {
            return;
        }

        for (int i = 0; i < 4; i++) {
            setScores(start + directionVectors.at(static_cast<Direction>(i)), end, currentScore + 1);
        }
    }

    void setScores() {
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                scores[x][y] = INT_MAX;
            }
        }

        setScores(start, end, 0);
    }

    // bool exitReachable(const Vec2& start) {
    //     if (start.x < 0 || start.x >= width || start.y < 0 || start.y >= height) {
    //         return false;
    //     }

    //     if (start == end) {
    //         return true;
    //     }

    //     // char& current = operator[](start);
    //     if (map[start.x][start.y] == '#' || map[start.x][start.y] == 'X') {
    //         return false;
    //     }

    //     map[start.x][start.y] = 'X';
    //     int i = 4;
    //     while (i-- > 0) {
    //         if (exitReachable(start + directionVectors.at(static_cast<Direction>(i)))) {
    //             map[start.x][start.y] = '.';
    //             return true;
    //         }
    //     }

    //     map[start.x][start.y] = '.';
    //     return false;
    // }

    bool exitReachableIterative() const {
        std::vector<Vec2> nextTiles = {start};
        std::set<Vec2> tilesVisited;

        while (nextTiles.size() > 0) {
            const Vec2 pos = nextTiles.back();
            nextTiles.pop_back();

            if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height) {
                continue;
            }

            if (pos == end) {
                return true;
            }

            tilesVisited.insert(pos);
            const char& current = operator[](pos);

            if (current == '.') {
                for (int i = 0; i < 4; i++) {
                    const Vec2& next = pos + directionVectors.at(static_cast<Direction>(i));
                    if (!tilesVisited.contains(next)) {
                        nextTiles.push_back(next);
                    }
                }
            }
        }

        return false;
    }

    std::ostream& printScores(std::ostream& os) const {
        for (int y = 0; y < height; y++) {
            os << "\n";

            for (int x = 0; x < width; x++) {
                os << (scores[x][y] == INT_MAX ? -1 : scores[x][y]) << "\t";
            }
        }

        return os;
    }

    std::ostream& printMap(std::ostream& os) const {
        for (int y = 0; y < height; y++) {
            os << "\n";

            for (int x = 0; x < width; x++) {
                os << map[x][y];
            }
        }

        return os;
    }

    std::ostream& printMap() const {
        return printMap(std::cout) << std::endl;
    }
};

std::ostream& operator<<(std::ostream& os, const Map& map) {
    return map.printMap(os);
}

void set_zero(int& a) {
    a = 0;
}

int main() {
    static constexpr int mapSize = 71;

    Map map = Map(mapSize, mapSize);
    map.start = Vec2(0, 0);
    map.end = Vec2(mapSize - 1, mapSize - 1);

    std::ifstream file("input.txt");
    std::string line;
    std::vector<Vec2> fallingBytes;

    int x, y;
    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        int index = line.find(',');
        x = std::stoi(line.substr(0, index));
        y = std::stoi(line.substr(index + 1));

        fallingBytes.emplace_back(x, y);
    }

    file.close();

    int bytesFelt = 0;
    while (bytesFelt < 1671) {
        map[fallingBytes[bytesFelt++]] = '#';
    }

    // std::cout << map << std::endl;
    map.setScores();
    // map.printScores(std::cout) << std::endl;
    std::cout << map.scores[map.end.x][map.end.y] << std::endl;

    while (map.exitReachableIterative()) {
        std::cout << bytesFelt << std::endl;

        map[fallingBytes[bytesFelt++]] = '#';
    }

    std::cout << fallingBytes[bytesFelt - 1] << std::endl;
}