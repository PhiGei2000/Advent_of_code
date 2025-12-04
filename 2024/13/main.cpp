#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct ClawMachine {
    int dxA, dyA;
    int dxB, dyB;

    long long prizeX, prizeY;

    ClawMachine(int dxA, int dyA, int dxB, int dyB, long long prizeX, long long prizeY)
        : dxA(dxA), dyA(dyA), dxB(dxB), dyB(dyB), prizeX(prizeX), prizeY(prizeY) {
    }

    long long getMinTokens() const {
        long long det = dxA * dyB - dyA * dxB;
        if (det == 0) {
            throw;
        }

        long long A = (prizeX * dyB - prizeY * dxB);
        long long B = (-prizeX * dyA + prizeY * dxA);

        if ((A * dxA + B * dxB) == det * prizeX && (A * dyA + B * dyB) == det * prizeY) {
            return 3 * std::div(A, det).quot + std::div(B, det).quot;
        }

        std::cout << "A: " << A << " B: " << B << " failed\n";
        return 0;
    }
};

int main(int argC, char** argV) {
    std::ifstream file("input.txt");

    std::string line;
    std::vector<ClawMachine> clawMachines;

    int dxA, dyA, dxB, dyB;
    long long prizeX, prizeY;
    while (std::getline(file, line)) {
        if (line.empty()) {
            clawMachines.emplace_back(dxA, dyA, dxB, dyB, prizeX, prizeY);
        }
        else if (line.starts_with("Button A:")) {
            int xIndex = line.find("X");
            dxA = std::stoi(line.substr(xIndex + 1));

            int yIndex = line.find("Y");
            dyA = std::stoi(line.substr(yIndex + 1));
        }
        else if (line.starts_with("Button B:")) {
            int xIndex = line.find("X");
            dxB = std::stoi(line.substr(xIndex + 1));

            int yIndex = line.find("Y");
            dyB = std::stoi(line.substr(yIndex + 1));
        }
        else if (line.starts_with("Prize")) {
            int xIndex = line.find("X=");
            prizeX = std::stoll(line.substr(xIndex + 2)) + 10000000000000;

            int yIndex = line.find("Y=");
            prizeY = std::stoll(line.substr(yIndex + 2)) + 10000000000000;
        }
    }

    if (!line.empty()) {
        clawMachines.emplace_back(dxA, dyA, dxB, dyB, prizeX, prizeY);
    }

    unsigned long long int totalTokens = 0;
    for (const auto& clawMachine : clawMachines) {
        totalTokens += clawMachine.getMinTokens();
    }

    std::cout << "Tokens needed: " << totalTokens << std::endl;
}

// 103082379222467 to low
// 160686558607435 false
// 160686558607382 false