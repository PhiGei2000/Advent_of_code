#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

struct Vec2 {
    int x;
    int y;

    constexpr bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
};

template<>
struct std::hash<std::pair<Vec2, char>> {
    std::size_t operator()(const std::pair<Vec2, char>& p) const {
        return std::hash<long>()((static_cast<long long>(p.first.x) << 40) + (p.first.y << 8) + p.second);
    }
};

struct Lab {
    char** lab;
    int labWidth;
    int labHeight;
    Vec2 guardPosition;
    std::unordered_set<std::pair<Vec2, char>> turns;

    Lab(char** lab, int labWidth, int labHeight, Vec2 guardPosition)
        : lab(lab), labHeight(labHeight), labWidth(labWidth), guardPosition(guardPosition) {
    }

    Lab(const Lab& lab)
        : labHeight(lab.labHeight), labWidth(lab.labWidth), guardPosition(lab.guardPosition) {
        this->lab = new char*[labWidth];
        for (int x = 0; x < labWidth; x++) {
            this->lab[x] = new char[labHeight];
            for (int y = 0; y < labHeight; y++) {
                this->lab[x][y] = lab.lab[x][y];
            }
        }
    }

    ~Lab() {
        for (int x = 0; x < labWidth; x++) {
            delete[] lab[x];
        }

        delete[] lab;
    }

    static Lab loadLab(const std::string& filename) {
        std::fstream file(filename);

        std::vector<std::string> lines;
        std::string line;

        while (std::getline(file, line)) {
            lines.push_back(line);
        }

        int lineLength = lines[0].size();

        Lab lab = {new char*[lineLength], lineLength, static_cast<int>(lines.size()), {0, 0}};
        for (int x = 0; x < lineLength; x++) {
            lab.lab[x] = new char[lines.size()];

            for (int y = 0; y < lines.size(); y++) {
                lab.lab[x][y] = lines[y][x];

                if (lines[y][x] == '^') {
                    lab.guardPosition = {x, y};
                }
            }
        }

        return lab;
    }

    bool isObstacle(int x, int y) const {
        if (x < 0 || y < 0 || x >= labWidth || y >= labHeight) {
            return false;
        }

        return lab[x][y] == '#';
    }

    bool guardInsideLab() const {
        return !(guardPosition.x < 0 || guardPosition.y < 0 || guardPosition.x >= labWidth || guardPosition.y >= labHeight);
    }

    bool addTurn(const Vec2& pos, const char guard) {
        return turns.emplace(std::make_pair(pos, guard)).second;
    }

    void moveGuard() {
        bool stop = !guardInsideLab();

        while (true) {
            char guard = lab[guardPosition.x][guardPosition.y];

            switch (guard) {
                case '^':
                    while (!isObstacle(guardPosition.x, guardPosition.y - 1)) {
                        lab[guardPosition.x][guardPosition.y] = 'X';
                        guardPosition.y--;
                        if (guardPosition.y >= 0 && !isObstacle(guardPosition.x, guardPosition.y)) {
                            lab[guardPosition.x][guardPosition.y] = '^';
                        }
                        else
                            break;
                    }

                    if (guardInsideLab()) {
                        lab[guardPosition.x][guardPosition.y] = '>';
                        if (!addTurn(guardPosition, guard)) {
                            return;
                        }
                    }
                    else {
                        return;
                    }
                    break;
                case '>':
                    while (!isObstacle(guardPosition.x + 1, guardPosition.y)) {
                        lab[guardPosition.x][guardPosition.y] = 'X';
                        guardPosition.x++;
                        if (guardPosition.x < labWidth  && !isObstacle(guardPosition.x, guardPosition.y)) {
                            lab[guardPosition.x][guardPosition.y] = '>';
                        }
                        else
                            break;
                    }

                    if (guardInsideLab()) {
                        lab[guardPosition.x][guardPosition.y] = 'v';
                        if (!addTurn(guardPosition, guard)) {
                            return;
                        }
                    }
                    else {
                        return;
                    }
                    break;
                case 'v':
                    while (!isObstacle(guardPosition.x, guardPosition.y + 1)) {
                        lab[guardPosition.x][guardPosition.y] = 'X';
                        guardPosition.y++;
                        if (guardPosition.y < labHeight && !isObstacle(guardPosition.x, guardPosition.y)) {
                            lab[guardPosition.x][guardPosition.y] = 'v';
                        }
                        else
                            break;
                    }

                    if (guardInsideLab()) {
                        lab[guardPosition.x][guardPosition.y] = '<';
                        if (!addTurn(guardPosition, guard)) {
                            return;
                        }
                    }
                    else {
                        return;
                    }
                    break;
                case '<':
                    while (!isObstacle(guardPosition.x - 1, guardPosition.y)) {
                        lab[guardPosition.x][guardPosition.y] = 'X';
                        guardPosition.x--;
                        if (guardPosition.x >= 0 && !isObstacle(guardPosition.x, guardPosition.y)) {
                            lab[guardPosition.x][guardPosition.y] = '<';
                        }
                        else
                            break;
                    }

                    if (guardInsideLab()) {
                        lab[guardPosition.x][guardPosition.y] = '^';
                        if (!addTurn(guardPosition, guard)) {
                            return;
                        }
                    }
                    else {
                        return;
                    }
                    break;
            }
        }
    }

    int countVisitedTiles() const {
        int tilesVisited = 0;
        for (int x = 0; x < labWidth; x++) {
            for (int y = 0; y < labHeight; y++) {
                if (lab[x][y] == 'X') {
                    tilesVisited++;
                }
            }
        }

        return tilesVisited;
    }
};

int main(int argC, char** argV) {
    Lab initialLab = Lab::loadLab("input.txt");
    Lab lab = Lab(initialLab);

    lab.moveGuard();

    int result = lab.countVisitedTiles();

    std::cout << "Tiles visited: " << result << std::endl;

    int closedPathsCount = 0;
    for (int x = 0; x < initialLab.labWidth; x++) {
        for (int y = 0; y < initialLab.labHeight; y++) {
            if (lab.lab[x][y] != 'X' || Vec2{x, y} == initialLab.guardPosition) {
                continue;
            }

            Lab modified = Lab(initialLab);
            modified.lab[x][y] = '#';

            modified.moveGuard();

            bool closedPath = modified.guardInsideLab();
            if (closedPath) {
                closedPathsCount++;
            }
        }
    }

    std::cout << "Closed paths: " << closedPathsCount << std::endl;
}
