//
// Created by youtous on 30/11/2019.
//

#ifndef INC_8INF856_PROJET_SUDOKU_H
#define INC_8INF856_PROJET_SUDOKU_H


#include <vector>
#include <exception>
#include <cmath>
#include <stdexcept>

class Sudoku {
private:
    std::vector<int> arrAsLine;
    int sizeN;
public:

    Sudoku(std::vector<int> &arrAsLine, int sizeN) : arrAsLine(std::move(arrAsLine)), sizeN(sizeN) {
        if (sizeN != sqrt(arrAsLine.size())) {
            throw std::invalid_argument("Size of the Sudoku must match with the given size.");
        }
    }

    inline const int *getRow(int i) const {
        return arrAsLine.data() + sizeN * i;
    }

    inline int *getRow(int i) {
        return arrAsLine.data() + sizeN * i;
    }

    inline std::vector<int> getRow() const {
        // todo : implements
    }

    inline std::vector<int> getColumn(int i) const {
        // todo : implements
        // return *(arrAsColumn.data() + sizeN * i);
    }

};


#endif //INC_8INF856_PROJET_SUDOKU_H
