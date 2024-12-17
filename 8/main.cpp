#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

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
};

Vec2 operator*(const int& scal, const Vec2& vec) {
    return Vec2(scal * vec.x, scal * vec.y);
}

struct Map;

std::ostream& operator<<(std::ostream&, const Map&);

struct Map {
    char** map = nullptr;
    std::map<char, std::set<Vec2>> antennaPositions;

    int width, height;

    static Map loadMap(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        Map result;

        std::vector<std::string> lines;
        while (std::getline(file, line)) {
            lines.emplace_back(line);
        }

        result.height = lines.size();
        result.width = lines[0].size();
        result.map = new char*[result.width];
        for (int x = 0; x < result.width; x++) {
            result.map[x] = new char[result.height];
            for (int y = 0; y < result.height; y++) {
                const char ch = lines[y][x];
                result.map[x][y] = ch;

                if (ch != '.') {
                    result.antennaPositions[ch].insert(Vec2(x, y));
                }
            }
        }

        return result;
    }

    void saveMap(const std::string& filename) {
        std::ofstream outputFile(filename);

        outputFile << *this;
        outputFile.close();
    }

    std::set<Vec2> getAntinodePositions() const {
        std::set<Vec2> antinodePositions;

        for (const auto& [freq, positions] : antennaPositions) {
            // loop through pairs
            for (auto it = positions.begin(); it != positions.end(); it++) {
                for (auto jt = positions.upper_bound(*it); jt != positions.end(); jt++) {
                    Vec2 difference = *it - *jt;

                    Vec2 antinode1 = *it;
                    while (antinode1.x >= 0 && antinode1.x < width && antinode1.y >= 0 && antinode1.y < height) {
                        antinodePositions.insert(antinode1);
                        if (map[antinode1.x][antinode1.y] == '.') {
                            map[antinode1.x][antinode1.y] = '#';
                        }

                        antinode1 += difference;
                    }

                    Vec2 antinode2 = *jt;
                    while (antinode2.x >= 0 && antinode2.x < width && antinode2.y >= 0 && antinode2.y < height) {
                        antinodePositions.insert(antinode2);
                        if (map[antinode2.x][antinode2.y] == '.') {
                            map[antinode2.x][antinode2.y] = '#';
                        }

                        antinode2 -= difference;
                    }
                }
            }
        }

        return antinodePositions;
    }
};

std::ostream& operator<<(std::ostream& os, const Map& map) {
    for (int y = 0; y < map.height; y++) {
        for (int x = 0; x < map.width; x++) {
            os << map.map[x][y];
        }
        os << "\r\n";
    }

    return os;
}

int main(int argC, char** argV) {
    Map map = Map::loadMap("input.txt");

    std::set<Vec2> antinodePositions = map.getAntinodePositions();

    std::cout << map << std::endl;
    std::cout << "Antinodes count: " << antinodePositions.size() << std::endl;
}