#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int quickSort(std::vector<int>& list, int start, int end) {
    if (start >= end) {
        return -1;
    }

    auto i = start;
    auto j = end - 1;

    int pivot = list[end];

    while (i < j) {
        while (i < j && list[i] <= pivot) {
            i++;
        }

        while (j > i && list[j] > pivot) {
            j--;
        }

        if (list[i] > list[j]) {
            int tmp = list[j];
            list[j] = list[i];
            list[i] = tmp;
        }
    }

    if (list[i] > pivot) {
        int tmp = list[end];
        list[end] = list[i];
        list[i] = tmp;
    }
    else {
        i = end;
    }

    quickSort(list, start, i - 1);
    quickSort(list, i + 1, end);

    return i;
}

int main(int argC, char** args) {
    std::vector<int> leftNumbers;
    std::vector<int> rightNumbers;

    std::fstream inputFile("input.txt");

    std::string line;
    while (std::getline(inputFile, line)) {
        std::stringstream lineStream(line);

        int left, right;
        lineStream >> left;
        lineStream >> right;

        leftNumbers.push_back(left);
        rightNumbers.push_back(right);
    }

    quickSort(leftNumbers, 0, leftNumbers.size() - 1);
    quickSort(rightNumbers, 0, rightNumbers.size() - 1);

    int distanceSum = 0;
    int similarityScore = 0;
    int j = 0;

    for (int i = 0; i < leftNumbers.size(); i++) {
        distanceSum += abs(leftNumbers[i] - rightNumbers[i]);

        int occurences = 0;
        while (j < rightNumbers.size() && rightNumbers[j] < leftNumbers[i]) {
            j++;
        }

        while (j < rightNumbers.size() && rightNumbers[j] == leftNumbers[i]) {
            occurences++;
            j++;
        }

        similarityScore += occurences * leftNumbers[i];
    }

    std::cout << "Sum of distances: " << distanceSum << std::endl;
    std::cout << "Similarity score: " << similarityScore << std::endl;
}