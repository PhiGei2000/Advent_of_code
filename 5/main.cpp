#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using PageOrderingRules = std::map<int, std::set<int>>;
using PrintQueue = std::vector<int>;

PageOrderingRules readPageOrderingRules(std::istream& stream) {
    std::string line;
    PageOrderingRules rules;

    while (std::getline(stream, line) && !line.empty()) {
        int splitIndex = line.find('|');
        int key = std::stoi(line.substr(0, splitIndex));
        int value = std::stoi(line.substr(splitIndex + 1));

        rules[key].emplace(value);
    }

    return rules;
}

std::vector<PrintQueue> readQueues(std::istream& stream) {
    std::string line;
    std::vector<PrintQueue> queues;

    while (std::getline(stream, line)) {
        int start = 0;
        PrintQueue queue;

        for (int i = line.find(','); i < line.length(); i = line.find(',', start)) {
            queue.push_back(std::stoi(line.substr(start, i - start)));
            start = i + 1;
        }
        queue.push_back(std::stoi(line.substr(start)));

        queues.push_back(queue);
    }

    return queues;
}

bool checkQueue(const PrintQueue& queue, const PageOrderingRules& rules) {
    for (int i = 1; i < queue.size(); i++) {
        int currentPage = queue[i];
        if (!rules.contains(currentPage))
            continue;

        for (int j = 0; j < i; j++) {
            if (rules.at(currentPage).contains(queue[j])) {
                return false;
            }
        }
    }

    return true;
}

void sortQueue(PrintQueue& queue, const PageOrderingRules& rules) {
    for (int i = 1; i < queue.size();) {
        int currentPage = queue[i];
        if (!rules.contains(currentPage)) {
            i++;
            continue;
        }

        int swapIndex = -1;
        for (int j = 0; j < i; j++) {
            if (rules.at(currentPage).contains(queue[j])) {
                queue[i] = queue[j];
                queue[j] = currentPage;
                swapIndex = j;

                break;
            }
        }

        if (swapIndex >= 0) {
            i = 1;
        }
        else {
            i++;
        }
    }
}

int main(int argC, char** argV) {
    std::fstream file("input.txt");

    PageOrderingRules rules = readPageOrderingRules(file);
    std::vector<PrintQueue> queues = readQueues(file);

    int sum = 0;
    int sumIncorrect = 0;
    for (int i = 0; i < queues.size(); i++) {
        PrintQueue& queue = queues[i];

        if (checkQueue(queue, rules)) {
            int middlePage = queue[queue.size() / 2];
            sum += middlePage;
        }
        else {
            sortQueue(queue, rules);

            if (!checkQueue(queue, rules)) {
                throw 1;
            }
            int middlePage = queue[queue.size() / 2];
            sumIncorrect += middlePage;
        }
    }

    std::cout << "Sum of middle page numbers: " << sum << std::endl;
    std::cout << "Sum of middle page numbers of incorrectly ordered queues after sorting: " << sumIncorrect << std::endl;
}
