//
// Created by youtous on 30/11/2019.
//

#ifndef INC_8INF856_PROJET_SUDOKU_H
#define INC_8INF856_PROJET_SUDOKU_H


#include <vector>
#include <cmath>

class Sudoku {
private:
    std::vector<int> arrAsLine;
    std::vector<int *> arrAsColumn;
    std::vector<int *> arrAsCell;
    int sizeN;
public:

    Sudoku(std::vector<int> &arrAsLine, int sizeN) : arrAsLine(std::move(arrAsLine)), sizeN(sizeN) {
        if(sizeN != sqrt(arrAsLine.size())) {
            // probleme !
        }

        arrAsColumn.resize(arrAsLine.size());
        for (int i = 0; i < arrAsLine.size(); ++i) {
            // todo : algo
            arrAsColumn[i] = (arrAsLine.data() + sizeN * i);
        }
    }

    inline const int *getRow(int i) const {
        return arrAsLine.data() + sizeN * i;
    }

    inline int *getRow(int i) {
        return arrAsLine.data() + sizeN * i;
    }

    inline const int *getColumn(int i) const {
        return *(arrAsColumn.data() + sizeN * i);
    }

    inline int *getColumn(int i) {
        return *(arrAsColumn.data() + sizeN * i);
    }

};


#endif //INC_8INF856_PROJET_SUDOKU_H
