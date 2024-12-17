#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

struct Equation {
    using NumberType = unsigned long long;

    NumberType result;
    std::vector<NumberType> terms;
    std::vector<char> operators;

    Equation()
        : result(0) {
    }

    Equation(NumberType result, const std::vector<NumberType>& terms)
        : result(result), terms(terms) {
    }

    static Equation parse(const std::string& string) {
        Equation eqn;

        int resultEnd = string.find(':');
        eqn.result = std::stoll(string.substr(0, resultEnd));

        int start = resultEnd + 2;
        int end = string.find(' ', start);
        while (start) {
            eqn.terms.push_back(std::stoll(string.substr(start, end - start)));

            start = end + 1;
            end = string.find(' ', start);
        }

        return eqn;
    }

    NumberType getResult() const {
        NumberType result = terms[0];
        for (int i = 1; i < terms.size(); i++) {
            switch (operators[i - 1]) {
                case '+':
                    result += terms[i];
                    break;
                case '*':
                    result *= terms[i];
                    break;
                case '|': {
                    int factor = 10;
                    while(factor <= terms[i]) {
                        factor *= 10;
                    }

                    result = result * factor + terms[i];
                } break;
                default:
                    break;
            }
        }

        return result;
    }

    bool isValid() const {
        return result == getResult();
    }

    bool determineOperators() {
        operators = std::vector<char>(terms.size() - 1, '+');

        bool eqnValid = isValid();
        bool end = false;
        while (!eqnValid && !end) {
            // update operators
            bool carry = false;
            int index = 0;

            do {
                switch (operators[index]) {
                    case '+':
                        carry = false;
                        operators[index] = '*';
                        break;
                    case '*':
                        carry = false;
                        operators[index] = '|';
                        break;
                    case '|':
                        carry = true;
                        operators[index] = '+';
                        break;
                }

                index++;
            } while (carry && index < operators.size());

            end = carry;
            eqnValid = isValid();
        }

        return eqnValid;
    }
};

std::ostream& operator<<(std::ostream& str, const Equation& eqn) {
    str << eqn.result << " = " << eqn.terms[0];

    for (int i = 0; i < eqn.operators.size(); i++) {
        str << " " << eqn.operators[i] << " " << eqn.terms[i + 1];
    }

    return str;
}

int main(int argC, char** argV) {
    std::ifstream file("input.txt");

    std::vector<Equation> equations;

    std::string line;
    while (std::getline(file, line)) {
        equations.push_back(Equation::parse(line));
    }

    unsigned long long int totalResult = 0;
    for (auto& equation : equations) {
        if (equation.determineOperators()) {
            totalResult += equation.result;
            std::cout << equation << " valid\r\n";
        }
        else {
            std::cout << equation << " not valid\r\n";
        }
    }

    std::cout << "Total calibration value: " << totalResult << std::endl;
}
