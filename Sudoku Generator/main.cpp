#include "sudoku-gen.h"

#include <iostream>
#include <thread>

void printUsageError(char *programName) {
    std::cerr << "Usage: " << programName << " cell_size board_count" << '\n';
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4 || atoi(argv[1]) < 3) {
        printUsageError(argv[0]);
        return 1;
    }

    auto iterations = atol(argv[2]);
    auto start_time = std::chrono::system_clock::now();
    std::stringstream output;

    srand(time(NULL));


    for (auto i = 0; i < iterations; ++i) {
        auto board = generateAndFillBoard(atoi(argv[1]));
        output << board.serialize() << '\n';
        board.writeInFile();
    }

    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time).count();
    auto boards_per_s = 10e5 * ((double) iterations) / duration;
    std::cout << "time micros: " << duration << '\n';
    std::cout << "Iterations " << iterations << '\n';
    std::cout << "boards per second " << boards_per_s << '\n';
    std::cout << output.str();

    return 0;
}
