#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct FileSystem {
    unsigned int totalBlocks = 0;
    std::vector<int> blocks;

    FileSystem(const std::vector<int>& diskMap) {
        for (auto it = diskMap.begin(); it != diskMap.end(); it++) {
            totalBlocks += *it;
        }

        blocks.reserve(totalBlocks);
        bool file = true;
        short fileId = 0;
        unsigned int offset = 0;

        for (auto it = diskMap.begin(); it != diskMap.end(); it++) {
            int blocksCount = *it;

            if (file) {
                for (int i = 0; i < blocksCount; i++) {
                    blocks.emplace_back(fileId);
                }

                fileId++;
            }
            else {
                for (int i = 0; i < blocksCount; i++) {
                    blocks.emplace_back(-1);
                }
            }

            file = !file;

            offset += blocksCount;
        }
    }

    int getFirstFreeBlock(int startIndex = 0) const {
        int index = startIndex;
        while (index < totalBlocks && blocks[index] != -1) {
            index++;
        }

        return index;
    }

    int getLastFileBlock(int startIndex = -1) const {
        int index = startIndex >= 0 ? startIndex : totalBlocks - 1;
        while (index >= 0 && blocks[index] == -1) {
            index--;
        }

        return index;
    }

    int getFileSize(unsigned int fileStart) const {
        int size = 1;
        int fileId = blocks[fileStart];

        while (fileStart + size < totalBlocks && blocks[fileStart + size] == fileId) {
            size++;
        }

        return size;
    }

    int getFileBlockByID(int fileID) const {
        unsigned int fileStart = 0;

        while (blocks[fileStart] != fileID) {
            fileStart++;
        }

        return fileStart;
    }

    // void compress() {
    //     int freeBlock = getFirstFreeBlock();
    //     int lastFileBlock = getLastFileBlock();

    //     while (freeBlock < lastFileBlock) {
    //         blocks[freeBlock] = blocks[lastFileBlock];
    //         blocks[lastFileBlock] = -1;

    //         freeBlock = getFirstFreeBlock(freeBlock);
    //         lastFileBlock = getLastFileBlock(lastFileBlock);
    //     }
    // }

    void compress() {
        int fileStart = getLastFileBlock();
        int fileID = blocks[fileStart];

        while (fileID >= 0) {
            fileStart = getFileBlockByID(fileID);
            int fileSize = getFileSize(fileStart);

            unsigned int freeSpace = getFirstFreeBlock();
            int freeSpaceSize = getFileSize(freeSpace);

            while (freeSpace < fileStart && freeSpaceSize < fileSize) {
                freeSpace = getFirstFreeBlock(freeSpace + freeSpaceSize);
                freeSpaceSize = getFileSize(freeSpace);
            }

            if (freeSpaceSize >= fileSize && freeSpace < fileStart) {
                // move file
                for (int i = 0; i < fileSize; i++) {
                    blocks[freeSpace + i] = fileID;
                    blocks[fileStart + i] = -1;
                }
            }

            fileID--;
        }
    }

    std::size_t calculateChecksum() const {
        std::size_t checksum = 0;

        for (int index = 0; index < totalBlocks; index++) {
            if (blocks[index] == -1)
                continue;

            checksum += index * blocks[index];
        }

        return checksum;
    }
};

int main(int argC, char** argV) {
    std::vector<int> diskMap;
    std::ifstream file("input.txt");

    char current;
    while (file.get(current)) {
        diskMap.push_back(current - '0');
    }
    file.close();

    FileSystem fs = FileSystem(diskMap);

    fs.compress();

    std::size_t checksum = fs.calculateChecksum();

    std::cout << "Compressed filesystem checksum: " << checksum << std::endl;
}