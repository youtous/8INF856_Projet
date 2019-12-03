//
// Created by youtous on 30/11/2019.
//

#include <sstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "sudoku.h"

int main() {
    std::vector<int> testSudoku;
    int n = 6;
    int n2 = n * n;
    int n4 = n2 * n2;
    testSudoku.resize(n4);

    Sudoku sudoku(testSudoku, n);
    for (int x = 0; x < n; ++x) {
        for (int y = 0; y < n; ++y) {
            // work on a single cell
            int start = 1;
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    sudoku[x * n + i][y * n + j] = start++;
                }
            }
        }
    }

    std::cout << "Affichage du sudoku :" << std::endl
              << sudoku << std::endl;;
    return 0;
}

Sudoku::Sudoku(std::vector<int> &initArr, int n) : arrAsLine(std::move(initArr)), n(n), rows(n * n), cols(n * n) {
    double squaredNCheck = sqrt(sqrt(this->arrAsLine.size()));
    if (((double) this->n) != squaredNCheck) {
        std::stringstream errMsg;
        errMsg << "Size of the Sudoku is " << this->rows << "x" << this->cols <<
               " does not match with the square of given array : "
               << squaredNCheck;
        throw std::invalid_argument(errMsg.str());
    }

}

// Begin of data access methods

int const &Sudoku::get(int row, int col) const {
    if (row > this->rows - 1 || col > this->cols - 1) {
        std::stringstream ss;
        ss << "Trying to get [" << row << "," << col << "] on a [" << rows << "," << cols << "] matrix";
        throw std::out_of_range(ss.str());
    }
    return this->arrAsLine[this->cols * row + col];
}

int &Sudoku::get(int row, int col) {
    return const_cast<int &>(const_cast<const Sudoku *>(this)->get(row, col));
}

int const &Sudoku::SudokuRow::operator[](int col) const {
    return parent.get(this->row, col);
}

int &Sudoku::SudokuRow::operator[](int col) {
    return parent.get(this->row, col);
}

std::vector<int> Sudoku::SudokuRow::vector() const {
    const int end = parent.getColumnSize() * (this->row + 1);
    return std::vector<int>(parent.arrAsLine.begin() + parent.getColumnSize() * this->row,
                            parent.arrAsLine.begin() + (end >= 0 ? end : 0));
}

// End of data access methods

// Copy data access methods

std::vector<int> Sudoku::getCopyRow(int y) const {
    // todo : implements
    return std::vector<int>();
}

std::vector<int> Sudoku::getCopyColumn(int x)  const {
    // todo : implements
    return std::vector<int>();
}

std::vector<int> Sudoku::getCopyCell(int x, int y)   const {
    // todo : implements
    return std::vector<int>();
}

// End of copy data access methods

// Begin format methods

std::ostream &Sudoku::to_ostream(std::ostream &os) const {
    std::stringstream ss;
    int digits = (floor(log10(this->n * this->n)) + 1) + 1;

    for (int j = 0; j < this->cols; ++j) {
        ss << (j % n == 0 ? "+-" : "") << std::setfill('-') << std::setw(digits) << '-'
           << (j == this->cols - 1 ? "+" : "");
    }
    ss << std::endl;
    for (int i = 0; i < this->rows; ++i) {
        if (i != 0 && i != this->cols - 1 && i % this->n == 0) {
            for (int j = 0; j < this->cols; ++j) {
                ss << (j % n == 0 ? "+-" : "") << std::setfill('-') << std::setw(digits) << '-'
                   << (j == this->cols - 1 ? "+" : "");
            }
            ss << std::endl;
        }
        ss << this->get(i) << std::endl;
    }

    for (int j = 0; j < this->cols; ++j) {
        ss << (j % n == 0 ? "+-" : "") << std::setfill('-') << std::setw(digits) << '-'
           << (j == this->cols - 1 ? "+" : "");
    }
    return os << ss.str();
}

std::ostream &Sudoku::SudokuRow::to_ostream(std::ostream &os) const {
    std::stringstream ss;

    // get the number of digits in a row using the max value which can fit in a cell
    // https://stackoverflow.com/questions/6655754/finding-the-number-of-digits-of-an-integer
    int digits = floor(log10(this->parent.n * this->parent.n)) + 1;
    for (int j = 0; j < this->parent.cols; ++j) {
        ss << (j % this->parent.n == 0 ? "| " : "") << std::setw(digits)
           << this->parent.get(this->row, j) << " ";
    }
    ss << "|";
    return os << ss.str();
}

// End of format methods
