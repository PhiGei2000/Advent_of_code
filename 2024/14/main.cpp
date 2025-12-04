#include <Vec2.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

struct Robot {
    Vec2 position;
    Vec2 velocity;

    Robot(const Vec2& position, const Vec2& velocity)
        : position(position), velocity(velocity) {
    }

    void move() {
        position += velocity;
    }
};

struct Bathroom {
    std::vector<Robot> robots;
    int width = 101;
    int height = 103;

    void moveRobots() {
        for (Robot& robot : robots) {
            robot.move();

            if (robot.position.x < 0) {
                robot.position.x += width;
            }
            else if (robot.position.x >= width) {
                robot.position.x -= width;
            }

            if (robot.position.y < 0) {
                robot.position.y += height;
            }
            else if (robot.position.y >= height) {
                robot.position.y -= height;
            }
        }
    }

    bool symmetric() const {
        std::map<int, int> xValues;

        for (const auto& robot : robots) {
            xValues[robot.position.y] += robot.position.x;
        }

        for (const auto& [y, xSum] : xValues) {
            if (xSum != width / 2) {
                return false;
            }
        }
        return true;
    }

    int calculateSafetyFactor() const {
        int robotsCount[4] = {0, 0, 0, 0};

        for (const Robot& robot : robots) {
            if (robot.position.x < width / 2) {
                if (robot.position.y < height / 2) {
                    robotsCount[0]++;
                }
                else if (robot.position.y >= height / 2 + 1) {
                    robotsCount[1]++;
                }
            }
            else if (robot.position.x >= width / 2 + 1) {
                if (robot.position.y < height / 2) {
                    robotsCount[2]++;
                }
                else if (robot.position.y >= height / 2 + 1) {
                    robotsCount[3]++;
                }
            }
        }

        return robotsCount[0] * robotsCount[1] * robotsCount[2] * robotsCount[3];
    }
};

char* pixels = new char[101 * 103];

void displayFrame(const std::string& filename, const Bathroom& bathroom) {
    std::map<Vec2, int> robots;
    for (const Robot& robot : bathroom.robots) {
        robots[robot.position]++;
    }

    for (int x = 0; x < 101; x++) {
        for (int y = 0; y < 103; y++) {
            pixels[x * 101 + y] = robots[Vec2(x, y)] > 0 ? 0xFF : 0x00;
        }
    }

    stbi_write_bmp(filename.c_str(), 101, 103, 1, pixels);
}

int main(int argC, char** argV) {

    std::ifstream file("input.txt");

    std::string line;
    Bathroom bathroom;

    Vec2 pos, vel;
    while (std::getline(file, line)) {
        int index = line.find("p=") + 2;
        line = line.substr(index);
        pos.x = std::stoi(line);

        index = line.find(',') + 1;
        line = line.substr(index);
        pos.y = std::stoi(line);

        index = line.find("v=") + 2;
        line = line.substr(index);
        vel.x = std::stoi(line);

        index = line.find(',') + 1;
        line = line.substr(index);
        vel.y = std::stoi(line);

        bathroom.robots.emplace_back(pos, vel);
    }

    for (int i = 0; i < 15000; i++) {
        bathroom.moveRobots();

        if (i >= 10000) {
            std::string filename = std::to_string(i) + ".bmp";
            displayFrame(filename, bathroom);
        }
    }

    delete[] pixels;

    std::cout << "Safety factor: " << bathroom.calculateSafetyFactor() << std::endl;
}