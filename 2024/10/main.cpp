#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct HeightMap {
    using Point = std::pair<int, int>;

    int** heightValues;
    int width, height;

    HeightMap(const std::string& filename) {
        std::ifstream file(filename);

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        width = lines.front().size();
        height = lines.size();
        heightValues = new int*[width];
        for (int x = 0; x < width; x++) {
            heightValues[x] = new int[height];
            for (int y = 0; y < height; y++) {
                heightValues[x][y] = lines[y][x] - '0';
            }
        }
    }

    ~HeightMap() {
        for (int x = 0; x < width; x++) {
            delete[] heightValues[x];
        }

        delete[] heightValues;
    }

    std::vector<Point> getTrialheads(int x, int y, int lastX = -1, int lastY = -1) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return {};
        }

        int lastValue = -1;
        if (lastX >= 0 && lastX < width && lastY >= 0 && lastY < height) {
            lastValue = heightValues[lastX][lastY];
        }

        int heightValue = heightValues[x][y];
        if (heightValue != lastValue + 1) {
            return {};
        }

        if (heightValue == 9) {
            return std::vector<Point>({std::make_pair(x, y)});
        }

        std::vector<Point> points;
        if (x - 1 != lastX || lastX == -1) {
            points.append_range(getTrialheads(x - 1, y, x, y));
        }
        if (x + 1 != lastX || lastX == -1) {
            points.append_range(getTrialheads(x + 1, y, x, y));
        }

        if (y - 1 != lastY || lastY == -1) {
            points.append_range(getTrialheads(x, y - 1, x, y));
        }
        if (y + 1 != lastY || lastY == -1) {
            points.append_range(getTrialheads(x, y + 1, x, y));
        }
        return points;
    }

    int getTrialheadScore(int x, int y, bool distinct = false) const {
        std::vector<Point> destinations = getTrialheads(x, y);

        if (!distinct) {
            return destinations.size();
        }

        int score = 0;
        for (int i = 0; i < destinations.size(); i++) {
            bool counted = false;

            for (int j = 0; j < i; j++) {
                if (destinations[i] == destinations[j]) {
                    counted = true;
                    break;
                }
            }

            if (!counted) {
                score++;
            }
        }

        return score;
    }

    int sumTrialheadScores(bool distinct = false) const {
        int score = 0;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (heightValues[x][y] != 0)
                    continue;

                score += getTrialheadScore(x, y, distinct);
            }
        }

        return score;
    }
};

int main(int argC, char** argV) {
    HeightMap map = HeightMap("input.txt");

    std::cout << "Sum of trialhead scores (distinct destinations): " << map.sumTrialheadScores(true) << std::endl;
    std::cout << "Sum of trialhead scores: " << map.sumTrialheadScores(false) << std::endl;
}