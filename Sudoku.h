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

    Sudoku(std::vector<int> &arrAsLine, int sizeN);

    inline const int *getFromRow(int i) const {
        return arrAsLine.data() + sizeN * i;
    }

    inline int *getFromRow(int i) {
        return arrAsLine.data() + sizeN * i;
    }

    // todo : implements
    std::vector<int> getRow(int y) const;

    // todo : implements
    // return *(arrAsColumn.data() + sizeN * i);
    std::vector<int> getColumn(int x) const;

    std::vector<int> getCell(int x, int y) const;

};


#endif //INC_8INF856_PROJET_SUDOKU_H
