//
// Created by youtous on 30/11/2019.
//

#include "Sudoku.h"

Sudoku::Sudoku(std::vector<int> &arrAsLine, int sizeN) : arrAsLine(std::move(arrAsLine)), sizeN(sizeN) {
    if (sizeN != sqrt(arrAsLine.size())) {
        throw std::invalid_argument("Size of the Sudoku must match with the given size.");
    }
}
