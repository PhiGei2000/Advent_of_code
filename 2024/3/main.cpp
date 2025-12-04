#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

bool checkCharacter(std::istream& stream, char character) {
    bool result = stream.peek() == character;

    if (result)
        stream.get();

    return result;
}

bool checkString(std::istream& stream, const std::string& str) {
    for (const char ch : str) {
        if (!checkCharacter(stream, ch)) {
            return false;
        }
    }

    return true;
}

bool getNumber(std::istream& stream, int* result) {
    char next = stream.peek();
    if (next >= '0' && next <= '9') {
        stream >> *result;
        return *result < 1000;
    }

    return false;
}

void skipDisabled(std::stringstream& stream) {
    while (stream.tellg() != -1) {
        if (checkString(stream, "do()")) {
            return;
        }
        stream.get();
    }
}

int getNextMultiplication(std::stringstream& stream) {
    if (stream.tellg() == -1)
        return 0;

    // find the next m
    while (stream.tellg() != -1) {
        if (checkString(stream, "mul(")) {
            break;
        }
        else if (checkString(stream, "don't()")) {
            skipDisabled(stream);
        }
        else {
            stream.get();
        }
    }

    int first, second;
    if (!getNumber(stream, &first)) {
        return getNextMultiplication(stream);
    }

    if (!checkCharacter(stream, ',')) {
        return getNextMultiplication(stream);
    }

    if (!getNumber(stream, &second)) {
        return getNextMultiplication(stream);
    }

    if (!checkCharacter(stream, ')')) {
        return getNextMultiplication(stream);
    }

    return first * second;
}

int main(int argC, char** argV) {
    std::fstream inputFile("input.txt");
    std::stringstream stream;
    stream << inputFile.rdbuf();

    int sum = 0;
    while (stream.tellg() != -1) {
        sum += getNextMultiplication(stream);
    }

    std::cout << "Sum: " << sum << std::endl;
}

// 108165515
// 106828551
