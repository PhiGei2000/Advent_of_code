#include <fstream>
#include <iostream>
#include <string>
#include <vector>

char** readFile(const std::string& filename, int* linesCount, int* lineLength) {
    std::fstream file(filename);

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.emplace_back(line);
    }

    *linesCount = lines.size();
    *lineLength = lines[0].size();

    char** result = new char*[*lineLength];
    for (int i = 0; i < *lineLength; i++) {
        result[i] = new char[*linesCount];
        for (int j = 0; j < *linesCount; j++) {
            result[i][j] = lines[j][i];
        }
    }

    return result;
}

bool inRange(int x, int min, int max) {
    return x >= min && x < max;
}

bool checkCharacter(int x, int y, char character, char** characters, int linesCount, int lineLength) {
    if (!inRange(x, 0, lineLength) || !inRange(y, 0, linesCount)) {
        return false;
    }

    return characters[x][y] == character;
}

int countXMAS(char** characters, int linesCount, int lineLength) {
    int totalMatches = 0;

    for (int x = 0; x < lineLength; x++) {
        for (int y = 0; y < linesCount; y++) {
            if (characters[x][y] == 'X') {
                static constexpr char xmas[] = {'X', 'M', 'A', 'S'};
                int matchesCount = 8;

                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        if (dx == 0 && dy == 0)
                            continue;

                        for (int i = 1; i < 4; i++) {
                            if (!checkCharacter(x + i * dx, y + i * dy, xmas[i], characters, linesCount, lineLength)) {
                                matchesCount -= 1;
                                break;
                            }
                        }
                    }
                }

                totalMatches += matchesCount;
            }
        }
    }

    return totalMatches;
}

int countX_MAS(char** characters, int linesCount, int lineLength) {
    static constexpr char mas[] = {'M', 'A', 'S'};
    int totalMatches = 0;

    for (int y = 0; y < linesCount; y++) {
        for (int x = 0; x < lineLength; x++) {
            if (characters[x][y] != 'A')
                continue;

            int matches = 0;
            if (checkCharacter(x - 1, y - 1, 'M', characters, linesCount, lineLength) && checkCharacter(x + 1, y + 1, 'S', characters, linesCount, lineLength)) {
                matches += 1;
            }
            else if (checkCharacter(x - 1, y - 1, 'S', characters, linesCount, lineLength) && checkCharacter(x + 1, y + 1, 'M', characters, linesCount, lineLength)) {
                matches += 1;
            }

            if (checkCharacter(x - 1, y + 1, 'M', characters, linesCount, lineLength) && checkCharacter(x + 1, y - 1, 'S', characters, linesCount, lineLength)) {
                matches += 1;
            }
            else if (checkCharacter(x - 1, y + 1, 'S', characters, linesCount, lineLength) && checkCharacter(x + 1, y - 1, 'M', characters, linesCount, lineLength)) {
                matches += 1;
            }

            if (matches == 2) {
                totalMatches++;
            }
        }
    }

    return totalMatches;
}

int main(int argC, char** argV) {
    int linesCount, lineLength;
    char** characters = readFile("input.txt", &linesCount, &lineLength);

    std::cout << "Total \"XMAS\" matches: " << countXMAS(characters, linesCount, lineLength) << std::endl;

    std::cout << "Total \"X-MAS\" matches: " << countX_MAS(characters, linesCount, lineLength) << std::endl;

    for (int i = 0; i < lineLength; i++) {
        delete[] characters[i];
    }
    delete[] characters;
}