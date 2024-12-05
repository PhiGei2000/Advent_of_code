#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool isSave(const std::vector<int>& report, int removeIndex = -1) {
    bool increasing = false;
    bool decreasing = false;

    int lastNumber = removeIndex == 0 ? report[1] : report[0];
    int number;

    for (int i = (removeIndex == 0 ? 0 : 1); i < report.size(); i++) {
        if (removeIndex == i) {
            i++;
            continue;
        }

        number = report[i];

        int difference = number - lastNumber;

        if (difference < 0) {
            decreasing = true;
        }
        else if (difference > 0) {
            increasing = true;
        }
        else {
            return false;
        }

        if (abs(difference) > 3 || increasing && decreasing) {
            return false;
        }

        lastNumber = number;
    }

    return true;
}

int main(int argC, char** args) {
    std::fstream inputFile = std::fstream("input.txt");

    int saveReports = 0;
    int saveByRemove = 0;
    int totalReports = 0;

    std::string line;
    std::stringstream lineStream;
    std::vector<int> report = {};

    while (std::getline(inputFile, line)) {
        lineStream = std::stringstream(line);
        report.clear();

        int number;
        while (lineStream >> number) {
            report.push_back(number);
        }

        if (isSave(report)) {
            saveReports++;
        }
        else {
            int removedNumber;
            for (int i = 0; i < report.size(); i++) {
                removedNumber = report[i];
                report.erase(report.begin() + i);

                if (isSave(report)) {
                    saveReports++;
                    saveByRemove++;
                    break;
                }
                report.insert(report.begin() + i, removedNumber);
            }
        }

        totalReports++;

        std::cout << "Save reports: " << saveReports << " by remove: " << saveByRemove << "\tTotal reports: " << totalReports << std::endl;
    }
}