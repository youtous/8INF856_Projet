//
// Created by youtous on 30/11/2019.
//

#include <sstream>
#include <iostream>
#include "sudoku.h"

int main() {
    std::vector<int> testSudoku;
    int n = 3;
    int n2 = n * n;
    int n4 = n2 * n2;
    testSudoku.resize(n4);
    for (int i = 0; i < n4; ++i) {
        testSudoku[i] = i;
    }

    Sudoku sudoku(testSudoku, n);

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

// Begin format methods

std::ostream &Sudoku::to_ostream(std::ostream &os) const {
    std::stringstream ss;
    std::string separator;
    separator.insert(0, this->cols, '-');

    std::string lineSeparator("|");
    lineSeparator.insert(0, this->cols - 2, '-');
    lineSeparator.push_back('|');

    ss << separator << std::endl;
    for (int i = 0; i < this->rows; ++i) {
        ss << this->get(i) << std::endl;
        if (i != 0 && i != this->cols - 1 && i % this->cols == 0) {
            ss << lineSeparator << std::endl;
        }
    }
    ss << separator << std::endl;
    return os << ss.str();
}

std::ostream &Sudoku::SudokuRow::to_ostream(std::ostream &os) const {
    std::stringstream ss;
    ss << "|";
    for (int j = 0; j < this->parent.cols; ++j) {
        ss << this->parent.get(this->row, j) << " " << (j % this->parent.cols == 0 ? "|" : "");
    }
    return os << ss.str();
}

// End of format methods
