//
// Created by youtous on 30/11/2019.
//

#include <sstream>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "sudoku.h"

int main() {
    tests();
    return 0;
}

void tests() {
    int n = 3;
    int n2 = n * n;
    int n4 = n2 * n2;

    Sudoku sudoku(n);
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

    sudoku.setRow(2, {4, 3, 1, 2, 8, 9, 5, 6, 7});
    std::cout << "Affichage de la 3eme ligne :" << std::endl;
    std::stringstream ss;
    auto v = sudoku.getCopyRow(2);
    for (auto const &e: v) {
        ss << e << ",";
    }
    std::cout << ss.str() << std::endl;

    sudoku.setColumn(2, {6, 3, 1, 2, 8, 9, 5, 4, 7});
    std::cout << "Affichage de la 3eme colonnee :" << std::endl;
    std::stringstream ss2;
    auto vCol = sudoku.getCopyColumn(2);
    for (auto const &e: vCol) {
        ss2 << e << ",";
    }
    std::cout << ss2.str() << std::endl;

    sudoku.setCell(2, 2, {7, 3, 1, 2, 8, 9, 5, 4, 6});
    std::cout << "Affichage de la cellule (2,2):" << std::endl;
    std::stringstream ss3;
    auto vCell = sudoku.getCopyCell(2, 2);
    for (auto const &e: vCell) {
        ss3 << e << ",";
    }
    std::cout << ss3.str() << std::endl;


    std::cout << "Affichage du sudoku :" << std::endl
              << sudoku << std::endl;
}

Sudoku::Sudoku(int n) : arrAsLine(std::vector<int>(n * n * n * n)), n(n), rows(n * n), cols(n * n) {};

Sudoku::Sudoku(int n, std::vector<int> &&initArr) : arrAsLine(std::move(initArr)), n(n), rows(n * n), cols(n * n) {
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

std::vector<int> Sudoku::getCopyRow(int row) const {
    return {std::begin(this->arrAsLine) + row * this->cols, std::begin(this->arrAsLine) + (row + 1) * this->cols};
}

std::vector<int> Sudoku::getCopyColumn(int col) const {
    std::vector<int> copyColumn(this->rows);
    for (int i = 0; i < this->rows; ++i) {
        copyColumn[i] = this->arrAsLine[col + this->rows * i];
    }
    return copyColumn;
}

std::vector<int> Sudoku::getCopyCell(int cellX, int cellY) const {
    std::vector<int> copyCell(this->n * this->n);
    int i = 0;
    for (int x = cellX * this->n; x < (cellX + 1) * this->n; ++x) {
        for (int y = cellY * this->n; y < (cellY + 1) * this->n; ++y) {
            copyCell[i] = this->arrAsLine[this->cols * x + y];
            i += 1;
        }
    }
    return copyCell;
}

// End of copy data access methods

// Begin data setters methods
void Sudoku::setRow(int row, std::vector<int> const &rowVector) {
    if (this->cols != rowVector.size()) {
        std::stringstream ss;
        ss << "Set row " << row << " using a vector with a different size : row size = " << this->cols
           << ", vector size = " << rowVector.size();
        throw std::out_of_range(ss.str());
    }
    for (int i = 0; i < this->cols; ++i) {
        this->arrAsLine[row * this->cols + i] = rowVector[i];
    }
}

void Sudoku::setColumn(int col, std::vector<int> const &columnVector) {
    if (this->rows != columnVector.size()) {
        std::stringstream ss;
        ss << "Set column " << col << " using a vector with a different size : column size = " << this->rows
           << ", vector size = " << columnVector.size();
        throw std::out_of_range(ss.str());
    }
    for (int i = 0; i < this->rows; ++i) {
        this->arrAsLine[i * this->cols + col] = columnVector[i];
    }
}

void Sudoku::setCell(int cellX, int cellY, std::vector<int> const &cellVector) {
    if ((this->n * this->n) != cellVector.size()) {
        std::stringstream ss;
        ss << "Set cell (" << cellX << "," << cellY << ") using a vector with a different size : cell size = "
           << this->n * this->n
           << ", vector size = " << cellVector.size();
        throw std::out_of_range(ss.str());
    }
    int i = 0;
    for (int x = cellX * this->n; x < (cellX + 1) * this->n; ++x) {
        for (int y = cellY * this->n; y < (cellY + 1) * this->n; ++y) {
            this->arrAsLine[this->cols * x + y] = cellVector[i];
            i += 1;
        }
    }
}

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


std::string Sudoku::export_str() const {
    std::stringstream exportStr;
    exportStr << n;

    for (int i = 0; i < this->arrAsLine.size(); ++i) {
        if (i % this->cols == 0) {
            exportStr << std::endl;
        }
        exportStr << arrAsLine[i] << " ";

    }
    return exportStr.str();
}

// End of format methods
