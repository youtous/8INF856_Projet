//
// Created by youtous on 30/11/2019.
//

#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include "sudoku.h"

int main(int argc, char *argv[]) {
    int processId;                              /* Process rank */
    int countProcess;                           /* Number of processes */

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &countProcess);

    // does not work for the moment
    if (processId == 0) {
        solve();
    }

    // tests();


    MPI_Finalize();
    return 0;
}

void solve() {
    int processId;                              /* Process rank */
    int countProcess;                           /* Number of processes */

    bool isSolved = false;

    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &countProcess);

    SudokuBoard sudoku = createFromStdin();

    #pragma omp parallel shared(isSolved)
    {
        int threadId = omp_get_thread_num();
        int countThreads = omp_get_num_threads();


        std::cout << "m[" << processId << "]{" << threadId << "}" << "hello ! I have " << countThreads << " friends" <<
                  std::endl;
    }
}

void testFromStdin() {
    SudokuBoard sudoku = createFromStdin();

    std::cout << "Affichage du sudoku :" << std::endl
              << sudoku << std::endl;

}

void tests() {
    int n = 3;
    int n2 = n * n;
    int n4 = n2 * n2;

    SudokuBoard sudoku(n);
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
    std::cout << "Affichage du block (2,2):" << std::endl;
    std::stringstream ss3;
    auto vBlock = sudoku.getCopyBlock(2, 2);
    for (auto const &e: vBlock) {
        ss3 << e << ",";
    }
    std::cout << ss3.str() << std::endl;


    std::cout << "Affichage du sudoku :" << std::endl
              << sudoku << std::endl;

    std::cout << "Write in grille.txt" << std::endl;
    writeInFile("grille.txt", sudoku.export_str());

    std::cout << "Load from file a copy of the sudoku" << std::endl;
    SudokuBoard sudokuFromFile = createFromFile("grille.txt");
    std::cout << "Affichage du sudoku copié :" << std::endl
              << sudokuFromFile << std::endl;
}

// Begin of Solver methods
void solveBoard(SudokuBoard &board, int row, int col) {
    // current cell computed
    const int index = row * board.getRowSize() + col;

    // check end reached => terminate recursion
    if (index >= board.getSize()) {
        // the board is solved
        // todo : add it to the stack
        return;
    }
    // keep current cell value in memory
    int savedValue = board[row][col];

    // try all possible numbers in the cell
    for (int i = 1; i <= board.getSudokuDimension(); ++i) {
        if (board.testValueInCell(row, col, i)) {
            int nextRow = row;
            int nextCol = col;
            if (col + 1 == board.getRowSize()) {
                nextRow = std::min(row + 1, board.getColumnSize() - 1);
                nextCol = 0;
            } else {
                nextCol += 1;
            }

            // compute next cell
            solveBoard(board, nextRow, nextCol);
            board[row][col] = savedValue;
        }
    }
}

bool SudokuBoard::testValueInCell(int row, int col, int value) const {
    // same value
    if (this->get(row, col) == value) {
        return true;
    }
    // fixed value
    if (this->get(row, col) != 0) {
        return false;
    }

    // check in row
    for (int i = 0; i < this->getRowSize(); ++i) {
        if (this->get(row, i) == value) {
            return false;
        }
    }
    // check in column
    for (int j = 0; j < this->getColumnSize(); ++j) {
        if (this->get(j, col) == value) {
            return false;
        }
    }
    // check in block
    const int initBlockRow = this->getStartingRowBlockOfCell(row);
    const int initBlockCol = this->getStartingColBlockOfCell(col);
    for (int k = 0; k < this->getSudokuDimension(); ++k) {
        for (int p = 0; p < this->getSudokuDimension(); ++p) {
            if (this->get(initBlockRow + k, initBlockCol + p) == value) {
                return false;
            }
        }
    }

    // all tests passed!
    return true;
}

// End of Solver methods

SudokuBoard::SudokuBoard(int n) : arrAsLine(std::vector<int>(n * n * n * n)), n(n), rows(n * n), cols(n * n) {};

SudokuBoard::SudokuBoard(int n, std::vector<int> &&initArr) : arrAsLine(std::move(initArr)), n(n), rows(n * n),
                                                              cols(n * n) {
    double squaredNCheck = sqrt(sqrt(this->arrAsLine.size()));
    if (((double) this->n) != squaredNCheck) {
        std::stringstream errMsg;
        errMsg << "Size of the SudokuBoard is " << this->rows << "x" << this->cols <<
               " does not match with the square of given array : "
               << squaredNCheck;
        throw std::invalid_argument(errMsg.str());
    }

}

SudokuBoard createFromFile(std::string const &fileName) {
    std::ifstream inputFile;
    inputFile.open(fileName);

    if (inputFile.fail()) {
        throw std::iostream::failure("Could not open file.");
    }

    int n;
    inputFile >> n;

    SudokuBoard sudoku(n);

    int currentValue;
    for (int x = 0; x < sudoku.getColumnSize(); ++x) {
        for (int y = 0; y < sudoku.getRowSize(); ++y) {
            inputFile >> currentValue;
            sudoku[x][y] = currentValue;
        }
    }

    return sudoku;
}

SudokuBoard createFromStdin() {
    int val;
    std::cin >> val;
    SudokuBoard sudoku(val);

    for (int x = 0; x < sudoku.getColumnSize(); ++x) {
        for (int y = 0; y < sudoku.getRowSize(); ++y) {
            std::cin >> val;
            sudoku[x][y] = val;
        }
    }

    return sudoku;
}

// Begin of data access methods

int const &SudokuBoard::get(int row, int col) const {
    // todo : eventually disable
    if (row > this->rows - 1 || col > this->cols - 1) {
        std::stringstream ss;
        ss << "Trying to get [" << row << "," << col << "] on a [" << rows << "," << cols << "] matrix";
        throw std::out_of_range(ss.str());
    }
    return this->arrAsLine[this->cols * row + col];
}

int &SudokuBoard::get(int row, int col) {
    return const_cast<int &>(const_cast<const SudokuBoard *>(this)->get(row, col));
}

int const &SudokuBoard::SudokuRow::operator[](int col) const {
    return parent.get(this->row, col);
}

int &SudokuBoard::SudokuRow::operator[](int col) {
    return parent.get(this->row, col);
}

std::vector<int> SudokuBoard::SudokuRow::vector() const {
    const int end = parent.getColumnSize() * (this->row + 1);
    return std::vector<int>(parent.arrAsLine.begin() + parent.getColumnSize() * this->row,
                            parent.arrAsLine.begin() + (end >= 0 ? end : 0));
}

int SudokuBoard::getBlockRowOf(int row) const {
    return row / this->n;
}

int SudokuBoard::getBlockColOf(int col) const {
    return col / this->n;
}

int SudokuBoard::getStartingRowBlockOfCell(int row) const {
    return getBlockRowOf(row) * this->n;
}

int SudokuBoard::getStartingColBlockOfCell(int col) const {
    return getBlockColOf(col) * this->n;
}
// End of data access methods

// Copy data access methods

std::vector<int> SudokuBoard::getCopyRow(int row) const {
    return {std::begin(this->arrAsLine) + row * this->cols, std::begin(this->arrAsLine) + (row + 1) * this->cols};
}

std::vector<int> SudokuBoard::getCopyColumn(int col) const {
    std::vector<int> copyColumn(this->rows);
    for (int i = 0; i < this->rows; ++i) {
        copyColumn[i] = this->arrAsLine[col + this->rows * i];
    }
    return copyColumn;
}

std::vector<int> SudokuBoard::getCopyBlock(int cellX, int cellY) const {
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
void SudokuBoard::setRow(int row, std::vector<int> const &rowVector) {
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

void SudokuBoard::setColumn(int col, std::vector<int> const &columnVector) {
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

void SudokuBoard::setCell(int cellX, int cellY, std::vector<int> const &cellVector) {
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

std::ostream &SudokuBoard::to_ostream(std::ostream &os) const {
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

std::ostream &SudokuBoard::SudokuRow::to_ostream(std::ostream &os) const {
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


std::string SudokuBoard::export_str() const {
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

void writeInFile(std::string const &fileName, std::string const &contentFile) {
    std::ofstream mFile;
    mFile.exceptions(std::ifstream::badbit);
    mFile.open(fileName);
    if (mFile.fail()) {
        throw std::iostream::failure("Could not open file.");
    }
    mFile << contentFile;
    mFile.close();
}

// End of format methods
