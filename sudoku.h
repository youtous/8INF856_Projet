//
// Created by youtous on 30/11/2019.
//

#ifndef INC_8INF856_PROJET_SUDOKU_H
#define INC_8INF856_PROJET_SUDOKU_H


#include <vector>
#include <cmath>
#include <stdexcept>

class Sudoku {
private:
    std::vector<int> arrAsLine;
    int n;
    int rows, cols;
public:
    Sudoku(std::vector<int> &initArr, int n);

    inline const int *getFromRow(int i) const {
        return arrAsLine.data() + cols * i;
    }

    inline int *getFromRow(int i) {
        return arrAsLine.data() + cols * i;
    }

    inline int getRowSize() { return rows; }

    inline int getColumnSize() { return cols; }

    // todo : implements
    std::vector<int> getRow(int y) const;

    // todo : implements
    // return *(arrAsColumn.data() + sizeN * i);
    std::vector<int> getColumn(int x) const;

    std::vector<int> getCell(int x, int y) const;

    /**
     * Safe getter on matrix data.
     * @param row
     * @param col
     * @return
     */
    int const &get(int row, int col) const;

    int &get(int row, int col);

    std::ostream &to_ostream(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const Sudoku &sudoku) {
        return sudoku.to_ostream(os);
    }

    /**
     * SudokuRow represents a row of the Matrix.
     */
    class SudokuRow {

        /**
         * Mama <3
         */
        friend class Sudoku;

    public:
        /**
         * Access a col of the row.
         * @param col
         * @return
         */
        inline int const &operator[](int col) const;

        inline int &operator[](int col);

        /**
         * @return - row as a vector.
         */
        inline std::vector<int> vector() const;

        /**
         * @return - row as raw pointer to data
         */
        inline int *data() {
            return (parent.arrAsLine.data() + parent.getColumnSize() * this->row);
        }

        /**
         * @return size of the line.
         */
        inline int size() const { return parent.getRowSize(); }

        /**
        * Print the row.
        * @param os
        * @return
        */
        std::ostream &to_ostream(std::ostream &os) const;

        friend std::ostream &operator<<(std::ostream &os, const SudokuRow &row) {
            return row.to_ostream(os);
        }

    private:
        SudokuRow(Sudoku &parent_, int row_) :
                parent(parent_),
                row(row_) {}

        SudokuRow(Sudoku const &parent_, int row_) :
                parent(const_cast<Sudoku &>(parent_)),
                row(row_) {}

        Sudoku &parent;
        int row;
    };

    /**
     * Access a row of the Matrix.
     */
    SudokuRow operator[](int row) {
        return this->get(row);
    }

    SudokuRow operator[](int row) const {
        return this->get(row);
    }

    SudokuRow get(int row) {
        return {*this, row};
    }

    SudokuRow get(int row) const {
        return {*this, row};
    }
};

#endif //INC_8INF856_PROJET_SUDOKU_H
