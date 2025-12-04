#include <Vec2.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define PART2

template<typename T>
using vector2D = std::vector<std::vector<T>>;

struct Warehouse {
    vector2D<char> map;
    int width, height;

    Vec2 robotPosition;

    char& operator[](const Vec2& pos) {
        return map[pos.y][pos.x];
    }

    const char& operator[](const Vec2& pos) const {
        return map[pos.y][pos.x];
    }

    bool isWall(const Vec2& pos) const {
        return operator[](pos) == '#';
    }

    bool isBox(const Vec2& pos) const {
#ifdef PART2
        return operator[](pos) == '[' || operator[](pos) == ']';
#else
        return operator[](pos) == 'O';
#endif
    }

    bool canMove(const Vec2& position, const Vec2& direction) const {
        if (isWall(position))
            return false;

        if (isBox(position)) {
#ifdef PART2
            if (direction.y != 0) {
                if (operator[](position) == '[') {
                    return canMove(position + direction, direction) && canMove(position + direction + Vec2(1, 0), direction);
                }
                else if (operator[](position) == ']') {
                    return canMove(position + direction, direction) && canMove(position + direction + Vec2(-1, 0), direction);
                }
            }
#endif
            return canMove(position + direction, direction);
        }

        return true;
    }

    void moveBox(const Vec2& position, const Vec2& direction) {
#ifdef PART2
        const Vec2& offset = direction * Vec2(2, 1);

        if (!isBox(position) || !canMove(position, direction)) {
            return;
        }

        Vec2 secondPos = operator[](position) == ']' ? position + Vec2(-1, 0) : position + Vec2(1, 0);
        char tmp1 = operator[](position), tmp2 = operator[](secondPos);

        moveBox(position + offset, direction);
        if (direction.y != 0) {
            moveBox(secondPos + offset, direction);
        }

        operator[](position + direction) = tmp1;
        operator[](secondPos + direction) = tmp2;

        if (secondPos != position + direction)
            operator[](secondPos) = '.';

        if (position != secondPos + direction)
            operator[](position) = '.';

#else
        if (!isBox(position) || !canMove(position, direction)) {
            return;
        }

        moveBox(position + direction, direction);
        operator[](position) = '.';
        operator[](position + direction) = 'O';
#endif
    }

    void moveRobot(char direction) {
        static const std::map<char, Vec2> directions = {
            std::make_pair('^', Vec2(0, -1)),
            std::make_pair('>', Vec2(1, 0)),
            std::make_pair('<', Vec2(-1, 0)),
            std::make_pair('v', Vec2(0, 1)),
        };

        Vec2 dir = directions.at(direction);
        if (!canMove(robotPosition + dir, dir)) {
            return;
        }

        moveBox(robotPosition + dir, dir);
        operator[](robotPosition) = '.';
        robotPosition = robotPosition + dir;
        operator[](robotPosition) = '@';
    }

    int sumBoxGpsCoords() const {
        int sum = 0;

        Vec2 pos;
        for (pos.x = 0; pos.x < width; pos.x++) {
            for (pos.y = 0; pos.y < height; pos.y++) {
                if (operator[](pos) == '[') {
                    sum += pos.y * 100 + pos.x;
                }
            }
        }

        return sum;
    }
};

int main() {
    Warehouse warehouse;
    warehouse.height = 0;

    // read warehouse
    std::ifstream file("input.txt");

    std::string line;
    int index;
    while (std::getline(file, line) && !line.empty()) {
#ifdef PART2
        std::vector<char> row;
        for (int i = 0; i < line.size(); i++) {
            char ch = line[i];
            switch (ch) {
                case '.':
                    row.push_back('.');
                    row.push_back('.');
                    break;
                case 'O':
                    row.push_back('[');
                    row.push_back(']');
                    break;
                case '#':
                    row.push_back('#');
                    row.push_back('#');
                    break;
                case '@':
                    row.push_back('@');
                    row.push_back('.');
                    warehouse.robotPosition = Vec2(2 * i, warehouse.map.size());
                    break;
            }
        }

        warehouse.map.push_back(row);
#else
        warehouse.map.push_back(std::vector<char>(line.begin(), line.end()));
        if ((index = line.find('@')) != std::string::npos) {
            warehouse.robotPosition = Vec2(index, warehouse.map.size() - 1);
        }
#endif

        warehouse.height++;
    }
    warehouse.width = warehouse.map.back().size();

    std::vector<char> moves;
    while (std::getline(file, line)) {
        moves.insert(moves.end(), line.begin(), line.end());
    }

    for (char move : moves) {
        warehouse.moveRobot(move);
    }

    std::cout << "Sum of GPS coords: " << warehouse.sumBoxGpsCoords() << std::endl;
}