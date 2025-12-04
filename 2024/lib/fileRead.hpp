#pragma once

#include <fstream>
#include <string>
#include <vector>

std::vector<std::string> readLines(const std::string& filename) {
    std::vector<std::string> lines;
    std::string line;

    std::ifstream file(filename);
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();
    return lines;
}
