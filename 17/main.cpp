#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

struct Computer {
    using Program = std::vector<int>;

    unsigned long long regA, regB, regC;

    Program program;
    unsigned int instructionPtr = 0;

    std::vector<int> output;

    Computer(const Program& program, unsigned long long registers[3])
        : program(program), regA(registers[0]), regB(registers[1]), regC(registers[2]) {
    }

    int getValue(int comboOperant) const {
        if (comboOperant <= 3) {
            return comboOperant;
        }

        switch (comboOperant) {
            case 4:
                return regA;
            case 5:
                return regB;
            case 6:
                return regC;
            default:
                throw;
        }
    }

    void adv(int comboOperant) {
        regA >>= getValue(comboOperant);
    }

    void bxl(int comboOperant) {
        regB ^= comboOperant;
    }

    void bst(int comboOperant) {
        regB = 0x07 & getValue(comboOperant);
    }

    bool jnz(int comboOperant) {
        if (regA == 0) {
            return false;
        }

        instructionPtr = comboOperant;
        return true;
    }

    void bxc(int comboOperant) {
        regB ^= regC;
    }

    void out(int comboOperant) {
        output.push_back(getValue(comboOperant) & 0x07);
    }

    void bdv(int comboOperant) {
        regB = regA >> getValue(comboOperant);
    }

    void cdv(int comboOperant) {
        regC = regA >> getValue(comboOperant);
    }

    void step() {
        int optCode = program[instructionPtr];
        int comboOperant = program[instructionPtr + 1];

        switch (optCode) {
            case 0:
                adv(comboOperant);
                break;
            case 1:
                bxl(comboOperant);
                break;
            case 2:
                bst(comboOperant);
                break;
            case 3:
                if (jnz(comboOperant)) {
                    // do not increase the instruction pointer
                    return;
                }
                break;
            case 4:
                bxc(comboOperant);
                break;
            case 5:
                out(comboOperant);
                break;
            case 6:
                bdv(comboOperant);
                break;
            case 7:
                cdv(comboOperant);
                break;
        }

        instructionPtr += 2;
    }

    void run() {
        while (instructionPtr < program.size()) {
            step();
        }
    }

    void reset(unsigned long long regValues[3]) {
        regA = regValues[0];
        regB = regValues[1];
        regC = regValues[2];

        instructionPtr = 0;
        output.clear();
    }
};

std::vector<std::array<int, 15>> getCoefficients(const std::vector<int>& seed) {

}

void part2(const Computer::Program& program) {
    unsigned long long A = 0;
    int table[8][8] = {
        {0, 1, 2, 3, 4, 5, 6, 7},
        {0, 2, 4, 6, 8, 10, 12, 14},
        {0, 4, 8, 12, 16, 20, 24, 28},
        {0, 8, 16, 24, 32, 40, 48, 56},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 1, 1, 1},
        {0, 0, 1, 1, 2, 2, 2, 2}};

    for (int i = 0; i < 15; i++) {
    }
}

int main(int argC, char** argV) {
    unsigned long long regValues[3];
    Computer::Program program;
    std::string programStr;

    std::ifstream file("input.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.starts_with("Register A:")) {
            regValues[0] = std::stoull(line.substr(12));
        }
        else if (line.starts_with("Register B:")) {
            regValues[1] = std::stoull(line.substr(12));
        }
        else if (line.starts_with("Register C:")) {
            regValues[2] = std::stoull(line.substr(12));
        }
        else if (line.starts_with("Program:")) {
            programStr = line.substr(9);

            int current = 0;
            int nextSeperator = programStr.find(',', current);
            int lastSeperator = 0;

            while (nextSeperator != std::string::npos) {
                program.push_back(std::stoi(programStr.substr(current, nextSeperator - current)));

                lastSeperator = nextSeperator;
                nextSeperator = programStr.find(',', lastSeperator + 1);
                current = lastSeperator + 1;
            }

            program.push_back(std::stoi(programStr.substr(current)));
        }
    }

    Computer pc(program, regValues);
    pc.run();

    for (const auto& out : pc.output) {
        std::cout << out << ",";
    }
    std::cout << std::endl;

    // part2(program);
    // part 2
    unsigned long long regA = 35188204800000;
    int index = 0;
    int increment = 1;
    bool match = false;

    while (!match) {
        regValues[0] = regA++;
        pc.reset(regValues);
        pc.output.reserve(programStr.size());

        while (pc.instructionPtr < pc.program.size()) {
            pc.step();
            if (program.size() < pc.output.size()) {
                break;
            }
        }

        match = true;
        if (program.size() != pc.output.size()) {
            match = false;
        }
        else {
            for (int i = 0; i < program.size() and match; i++) {
                if (program[i] != pc.output[i]) {
                    match = false;
                }
            }
        }

        if (regA % 100000 == 0) {
            std::cout << regA << std::endl;
        }
    }

    std::cout << "Min regA value: " << regValues[0] << std::endl;
}