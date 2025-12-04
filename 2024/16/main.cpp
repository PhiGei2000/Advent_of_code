#include <Vec2.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Map;

std::ostream& operator<<(std::ostream&, const Map&);

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

  private:
    void setScore(const Vec2& start, Direction dir, int score) {
        int& currentScore = scores[start.x][start.y];
        char& current = operator[](start);

        if (start == this->start) {
            currentScore = 0;
        }
        else if (current == '#' || currentScore < score) {
            return;
        }
        else {
            currentScore = score;
        }

        setScore(start + directionVectors.at(dir), dir, score + 1);
        setScore(start + directionVectors.at(turnLeft(dir)), turnLeft(dir), score + 1001);
        setScore(start + directionVectors.at(turnRight(dir)), turnRight(dir), score + 1001);
        setScore(start + directionVectors.at(-dir), -dir, score + 1);
    }

    void markPath(const Vec2& pos, Direction dir, bool turn = false) {
        if (pos == start)
            return;

        if (pos == end) {
            operator[](pos) = 'O';

            for (int i = 0; i < 4; i++) {
                markPath(pos + directionVectors.at(static_cast<Direction>(i)), static_cast<Direction>(i), false);
            }

            return;
        }

        int currValue = scores[pos.x][pos.y];
        if (currValue == INT_MAX) {
            return;
        }

        const Vec2& prev = pos + directionVectors.at(-dir);
        int prevValue = scores[prev.x][prev.y];

        Vec2 next = pos + (pos - prev);
        int nextValue = scores[next.x][next.y];

        if (currValue == prevValue - 1 || (nextValue == prevValue - 2 && currValue < prevValue) || (turn && currValue == prevValue - 1001)) {
            operator[](pos) = 'O';
        }
        else {
            return;
        }

        for (int i = 0; i < 4; i++) {
            Direction nextDir = static_cast<Direction>(i);
            if (dir == -nextDir)
                continue;

            markPath(pos + directionVectors.at(nextDir), static_cast<Direction>(nextDir), dir != nextDir);
        }
    }

  public:
    void
    setScores() {
        setScore(start, Direction::LEFT, 0);
    }

    void printScoreMap() const {
        for (int y = 0; y < height; y++) {
            std::cout << "\n";
            for (int x = 0; x < width; x++) {
                std::cout << "\t" << (scores[x][y] == INT_MAX ? -1 : scores[x][y]);
            }
        }

        std::cout << std::endl;
    }

    void markPaths() {
        markPath(end, Direction::UP, false);
    }
};

std::ostream& operator<<(std::ostream& os, const Map& map) {
    for (int y = 0; y < map.height; y++) {
        os << '\n';
        for (int x = 0; x < map.width; x++) {
            os << map.map[x][y];
        }
    }

    return os;
}

int main(int argC, char** argV) {
    std::ifstream file("input.txt");

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    Map map(lines.front().size(), lines.size());
    for (int x = 0; x < map.width; x++) {
        for (int y = 0; y < map.width; y++) {
            map.map[x][y] = lines[y][x];

            if (lines[y][x] == 'S') {
                map.start = Vec2(x, y);
            }
            else if (lines[y][x] == 'E') {
                map.end = Vec2(x, y);
            }
        }
    }

    map.setScores();
    map.printScoreMap();
    std::cout << "Min score: " << map.scores[map.end.x][map.end.y] << "\r\n";

    map.markPaths();
    std::cout << map << std::endl;
}