//
// Created by youtous on 30/11/2019.
//

#ifndef INC_8INF856_PROJET_SUDOKU_H
#define INC_8INF856_PROJET_SUDOKU_H


#include <vector>
#include <cmath>
#include <stdexcept>
#include <deque>
#include <utility>
#include <set>
#include <mpi.h>

#define CUSTOM_MPI_SOLUTIONS_TAG            10
#define CUSTOM_MPI_POSSIBILITIES_TAG            11
#define CUSTOM_MPI_IDLE_TAG            12
#define CUSTOM_MPI_STOP_WORK_TAG            13
#define CUSTOM_MPI_INVALID_SUDOKU_RETURNED            15


/**
 * SudokuBoard class represents a grid a the sudoku game.
 */
class SudokuBoard {
private:
    /**
     * Values of the grid are stored in row order.
     */
    std::vector<int> arrAsLine;
    /**
     * N is the complexity of the SudokuBoard. A sudoku grid contains n^2 cells,
     * each cell contains n^2 values.
     */
    int n;
    /**
     * Count how many rows and columns the SudokuBoard is made of.
     */
    int rows, cols;

    /**
     * Count how many cells are marked as solved.
     */
    int countSolvedCells = 0;

    /**
     * Mark if possibles values has been computed.
     */
    bool computedPossibleValues = false;

    /**
     * Store possibles values in each cell.
     */
    std::vector<std::vector<std::set<int>>> possiblesValuesInCells;
    /**
    * Store possibles values in each row.
    */
    std::vector<std::set<int>> possiblesValuesInRows;
    /**
    * Store possibles values in each column.
    */
    std::vector<std::set<int>> possiblesValuesInColumns;
    /**
    * Store possibles values in each block.
    */
    std::vector<std::set<int>> possiblesValuesInBlocks;
public:
    /**
     * SudokuBoard constructor.
     * @param n - size of the SudokuBoard
     * @param initArr - an initial array containing sudoku's grid values.
     */
    SudokuBoard(int n, std::vector<int> &&initArr);

    /**
     * SudokuBoard constructor.
     * @param n - size of the SudokuBoard
     */
    SudokuBoard(int n);

    /**
     * @param i - row index
     * @complexity - O(1) - constant
     * @return - raw pointer on the beginning at the row
     */
    inline const int *getFromRow(int i) const {
        return arrAsLine.data() + cols * i;
    }

    /**
     * @param i - row index
     * @complexity - O(1) - constant
     * @return - raw pointer on the beginning at the row
     */
    inline int *getFromRow(int i) {
        return arrAsLine.data() + cols * i;
    }

    /**
     * @complexity - O(1) - constant
     * @return - number of columns
     */
    inline int countColumns() const { return cols; }

    /**
     * @complexity - O(1) - constant
     * @return - number of rows
     */
    inline int countRows() const { return rows; }

    /**
     * @complexity - O(1) - constant
     * @return - number of blocks
     */
    inline int countBlocks() const { return rows; }


    /**
     * @complexity - O(1) - constant
     * @return - size of a row
     */
    inline int getRowSize() const { return rows; }

    /**
     * @complexity - O(1) - constant
     * @return - size of a column
     */
    inline int getColumnSize() const { return cols; }

    /**
     * @complexity - O(1) - constant
     * @return - size of a block
     */
    inline int getBlockSize() const { return cols; }

    /**
     * @complexity - O(1) - constant
     * @return - size of the board (number of cells)
     */
    inline int getSize() const { return rows * cols; }

    /**
     * @complexity - O(1) - constant
     * @return - dimension of the Sudoku, aka N
     */
    inline int getSudokuDimension() const { return n; }

    /**
     * @return - true if the board is empty
     */
    inline bool isEmpty() const { return n == 0; }

    /**
     * @return - how many cells in the board are solved.
     */
    int getCountSolvedCells() const;

    /**
     * Set how many cells are solved in the board.
     * @param countSolvedCells
     */
    void setCountSolvedCells(int countSolvedCells);

    /**
     * Count how many cells in the board have a non null value and save it.
     * @return - how many cells in the board are solved.
     */
    int recountSolvedCells();

    /**
     * Computes possibles values in each cell.
     */
    void computePossiblesValuesInCells();

    /**
     * @return - true if a computation was done, false otherwise.
     */
    bool isComputedPossibleValues() const;

    /**
     * @return - mapping of each possible values in each cell.
     */
    std::vector<std::vector<std::set<int>>> &getPossiblesValuesInCells();

    /**
     * @return - mapping of each possible values in each cell.
     */
    std::vector<std::vector<std::set<int>>> const &getPossiblesValuesInCells() const;

    /**
     * @return - mapping of each possible values in each row.
     */
    std::vector<std::set<int>> const &getPossiblesValuesInRows() const;

    /**
     * @return - mapping of each possible values in each row.
     */
    std::vector<std::set<int>> &getPossiblesValuesInRows();

    /**
     * @return - mapping of each possible values in each column.
     */
    std::vector<std::set<int>> const &getPossiblesValuesInColumns() const;

    /**
     * @return - mapping of each possible values in each column.
     */
    std::vector<std::set<int>> &getPossiblesValuesInColumns();

    /**
     * @return - mapping of each possible values in each block.
     */
    std::vector<std::set<int>> const &getPossiblesValuesInBlocks() const;

    /**
     * @return - mapping of each possible values in each block.
     */
    std::vector<std::set<int>> &getPossiblesValuesInBlocks();

    /**
     * Add a possible value for a given cell.
     * 0 is not a possible value.
     * @param row - row of the cell
     * @param col - col of the cell
     * @param value - value between 1 and getBlockSize
     */
    void addPossibleValueForCell(int row, int col, int value);

    /**
     * Remove a possible value for a given cell.
     * @param row - row of the cell
     * @param col - col of the cell
     */
    void removePossibleValueForCell(int row, int col, int value);

    /**
     * @return - true if the board is solved.
     */
    bool isSolved() const;

    /**
     * Check if a value can be set on a cell.
     * @param row - row of the cell
     * @param col - column of the cell
     * @param value - value wanted
     * @return true if the value can be set, false otherwise
     */
    bool testValueInCell(int row, int col, int value) const;

    /**
     * Check if a value can be set on a cell using computed available values.
     * @param row - row of the cell
     * @param col - column of the cell
     * @param value - value wanted
     * @return true if the value can be set, false otherwise
     */
    bool testValueInCellFromCompute(int row, int col, int value) const;

    /**
     * @param i - row index
     * @complexity - O(n) where n = number of columns
     * @return - a copy of the row
     */
    std::vector<int> getCopyRow(int row) const;

    /**
    * @param i - column index
    * @complexity - O(n) where n = number of rows
    * @return - a copy of the column
    */
    std::vector<int> getCopyColumn(int col) const;

    /**
    * @param cellX - block x index
    * @param cellY - block y index
    * @complexity - O(n) where n = sudoku n^2
    * @return - a copy of the block
    */
    std::vector<int> getCopyBlock(int cellX, int cellY) const;

    /**
     * @param row - row of the cell
     * @param col - col of the cell
     * @return - the index of the block
     */
    int getBlockOfCell(int row, int col) const;

    /**
     * @param row - row of the cell
     * @return - index of the first row of the block
     */
    int getBlockRowOf(int row) const;

    /**
     * @param col - column of the cell
     * @return - index of the first col of the block
     */
    int getBlockColOf(int col) const;

    /**
     * @param blockRow - row of the cell
     * @return - index of the first row of the block
     */
    int getStartingRowBlockOfCell(int row) const;

    /**
     * @param blockCol - column of the cell
     * @return - index of the first column of the block
     */
    int getStartingColBlockOfCell(int col) const;

    /**
     * @param row - index of the row to set
     * @param rowVector - values to set in the row
     * @complexity - O(n) where n = number of columns
     */
    void setRow(int row, std::vector<int> const &rowVector);

    /**
     * @param col - index of the column to set
     * @param columnVector - values to set in the column
     * @complexity - O(n) where n = number of rows
     */
    void setColumn(int col, std::vector<int> const &columnVector);

    /**
     * @param cellX - cell x index
     * @param cellY - cell y index
     * @param cellVector - values to set in the cell
     * @complexity - O(n) where n = sudoku n^2
     */
    void setCell(int cellX, int cellY, std::vector<int> const &cellVector);

    /**
     * Safe getter on matrix data.
     * @param row - row index
     * @param col - column index
     * @complexity - O(1) - constant
     * @return - value at given coordinates
     */
    int const &get(int row, int col) const;

    /**
     * Safe getter on matrix data.
     * @param row - row index
     * @param col - column index
     * @complexity - O(1) - constant
     * @return - value at given coordinates
     */
    int &get(int row, int col);

    /**
     * Set a value for a cell and update related possibles values.
     * @param row - row of the cell
     * @param col - col of the cell
     * @param value - value to set in the cell
     */
    void setValueAndUpdatePossibilities(int row, int col, int value);

    /**
     * Check a value is not duplicated in the row.
     * @param row - the row
     * @param value - the value
     * @return - true if not duplicated, false otherwise
     */
    bool checkNotDuplicatedInRow(int row, int value) const;

    /**
     * Check a value is not duplicated in the column.
     * @param col - the column
     * @param value - the value
     * @return - true if not duplicated, false otherwise
     */
    bool checkNotDuplicatedInCol(int col, int value) const;

    /**
     * Check a value is not duplicated in the block.
     * @param row - the row of the cell
     * @param col - the col of the cell
     * @param value - the value
     * @return - true if not duplicated, false otherwise
     */
    bool checkNotDuplicatedInBlock(int row, int col, int value) const;

    /**
     * Check a cell is not duplicated in row, block, column.
     * @param row - the row of the cell
     * @param col - the column of the cell
     * @return - true if not duplicated, false otherwise
     */
    bool checkNotDuplicatedCell(int row, int col) const;

    /**
     * @return - true if the current board has a valid configuration, false otherwise.
     */
    bool checkIsValidConfig() const;

    /**
     * @return - board as raw pointer to data
     */
    int *data() {
        return this->arrAsLine.data();
    }

    /**
     * @param os - stream used for output
     * @return - a stream containing the SudokuBoard in CLI
     */
    std::ostream &to_ostream(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, const SudokuBoard &sudoku) {
        return sudoku.to_ostream(os);
    }

    /**
     * @return - the SudokuBoard formated with a first value for N, then grid values
     */
    std::string export_str() const;

    /**
     * @complexity - O(n) - worst case where n = N
     * @return - x,y coordinates of the first empty cell found in the board
     *           if the board is complete, {-1, -1} will be returned
     */
    std::pair<int, int> nextEmptyCell() const;

    bool operator==(const SudokuBoard &rhs) const;

    bool operator!=(const SudokuBoard &rhs) const;

    /**
     * SudokuRow represents a row of the Matrix.
     */
    class SudokuRow {

        /**
         * Mama <3
         */
        friend class SudokuBoard;

    public:
        /**
         * Access a column of the row.
         *
         * @complexity - O(1) - constant
         * @param col - column in this row
         * @return - value at the given coordinates
         */
        inline int const &operator[](int col) const;

        /**
         * Access a column of the row.
         *
         * @complexity - O(1) - constant
         * @param col - column in this row
         * @return - value at the given coordinates
         */
        inline int &operator[](int col);

        /**
         * @complexity - O(n) where n = number of columns in this Row
         * @return - row as a vector.
         */
        inline std::vector<int> vector() const;

        /**
         * @complexity - O(1) - constant
         * @return - Row as raw pointer to data
         */
        inline int *data() {
            return (parent.arrAsLine.data() + parent.getColumnSize() * this->row);
        }

        /**
         * @complexity - O(1) - constant
         * @return size of the line.
         */
        inline int size() const { return parent.getRowSize(); }

        /**
        * Print the row.
        * @param os - stream used for output
        * @return - a stream containing the row in CLI
        */
        std::ostream &to_ostream(std::ostream &os) const;

        friend std::ostream &operator<<(std::ostream &os, const SudokuRow &row) {
            return row.to_ostream(os);
        }

    private:
        /**
         * SudokuRow constructor.
         * @param parent_  - parent of this row
         * @param row_ - row index in the SudokuBoard grid
         */
        SudokuRow(SudokuBoard &parent_, int row_) :
                parent(parent_),
                row(row_) {}

        /**
         * SudokuRow constructor.
         * @param parent_  - parent of this row
         * @param row_ - row index in the SudokuBoard grid
         */
        SudokuRow(SudokuBoard const &parent_, int row_) :
                parent(const_cast<SudokuBoard &>(parent_)),
                row(row_) {}

        /**
         * Parent of this Row.
         */
        SudokuBoard &parent;

        /**
         * Row index in the SudokuBoard grid.
         */
        int row;
    };

    /**
     * @param row - row index in the grid
     * @return - row of the grid
     */
    SudokuRow operator[](int row) {
        return this->get(row);
    }

    /**
     * @param row - row index in the grid
     * @return - row of the grid
     */
    SudokuRow operator[](int row) const {
        return this->get(row);
    }

    /**
     * @param row - row index in the grid
     * @return - row of the grid
     */
    SudokuRow get(int row) {
        return {*this, row};
    }

    /**
     * @param row - row index in the grid
     * @return - row of the grid
     */
    SudokuRow get(int row) const {
        return {*this, row};
    }
};

/**
 * Create a SudokuBoard from a saved instance contained in a file.
 *
 * @param fileName - file containing SudokuBoard's data.
 * @return - SudokuBoard initiated from the File.
 */
SudokuBoard createFromFile(std::string const &fileName);

/**
 * Create a SudokuBoard from an stdin, the first element contains
 * the size of the SudokuBoard.
 *
 * @return - SudokuBoard initiated from the File.
 */
SudokuBoard createFromStdin();

/**
 * Write in a File a given content.
 *
 * @param fileName - path of the file to write.
 * @param contentFile - content of the file to set.
 */
void writeInFile(std::string const &fileName, std::string const &contentFile);


/**
 * Solve a given SudokuBoard.
 * This function init board computation for solveBoard.
 *
 * @param board - the board to solve
 * @param solutionFound - a flag used to stop recursion
 * @return - the solved board if solved or a SudokuBoard with a 0 size if not solved
 */
SudokuBoard solveBoardRecursive(SudokuBoard &board, bool &solutionFound);

/**
 * Solve a given SudokuBoard.
 * This function is recursive, it should be started using default
 * row and col values (beginning of the board).
 *
 *
 * @param board - the board to solve
 * @param solutionFound - a flag used to stop recursion
 * @param row - (optional) the row of the cell to work on
 * @param col - (optional) the column of the cell to work on
 * @return - the solved board if solved or a SudokuBoard with a 0 size if not solved
 */
SudokuBoard solveBoard(SudokuBoard &board, bool &solutionFound, int row = 0, int col = 0);

/**
 * Solve a given SudokuBoard using Crook's algorithm.
 *
 * @param board - the board to solve, will be set empty if the algorithm discovered a dead-end
 * @param solutionFound - a flag used to stop recursion
 * @return - the solved board if solved or a SudokuBoard with a 0 size if not solved
 */
SudokuBoard solveReduceCrook(SudokuBoard &board, bool &solutionFound);

/**
 * Generate possibilities for the next empty cell of the front board to work.
 * If the board is complete, return the solution.
 * If the board is invalid, remove it.
 * Otherwise, generate a subproblem : each possible value in the cell will result in a new
 *            board to process.
 *
 * This function permits to prepare division of the work.
 *
 * @param boardsToWork - list of board to check.
 * @param solutions - list of possible boards solutions. If the board is valid, the board is added to solutions.
 * @return - the solved board if solved or a SudokuBoard with a 0 size if not solved
 */
SudokuBoard generatePossibilitiesNextCell(std::deque<SudokuBoard> &boardsToWork);

/**
 * On the process node, divide problems in sub-problems and run them on different threads.
 *
 * @param problems - problems to solve
 * @return - the solved board if solved or a SudokuBoard with a 0 size if not solved
 */
SudokuBoard solveProblemsOnNode(std::deque<SudokuBoard> &problems);

/**
 * Apply elimination strategy on the SudokuBoard.
 * @param board - the board.
 * @return - number of changed cells or -1 if the board is discovered as not valid.
 */
int eliminatationStrategy(SudokuBoard &board);

/**
 * Apply lone ranger strategy on the SudokuBoard.
 * @param board - the board.
 * @return - number of changed cells or -1 if the board is discovered as not valid.
 */
int lonerangerStrategy(SudokuBoard &board);

/**
 * Apply twins strategy on the SudokuBoard.
 * @param board - the board.
 * @return - number of changed cells or -1 if the board is discovered as not valid.
 */
int twinsStrategy(SudokuBoard &board);

/**
 * Apply triplets strategy on the SudokuBoard.
 * @param board - the board.
 * @return - number of changed cells or -1 if the board is discovered as not valid.
 */
int tripletsStrategy(SudokuBoard &board);

/**
 * Receive a std::dequeue<SudokuBoard> using MPI and push it at the end of the given queue.
 *
 * @complexity - O(n) where n = count received boards
 *
 * @param dequeue - the receiving queue
 * @param src - from
 * @param tag - MPI_TAG
 * @param comm - MPI_COM
 * @return - how many boards has been added to the dequeues
 */
int receivePushBackDeque(std::deque<SudokuBoard> &dequeue, int src, int tag, MPI_Comm comm);

/**
 * Send a std::deque<int> using MPI. The queue will be consumed!
 *
 * @complexity - O(n) where n = count send boards
 *
 * @param dequeue - the receiving queue
 * @param dest - to
 * @param tag - MPI_TAG
 * @param comm - MPI_COMM
 * @param count - (optional) define how many items to send
 */
void sendAndConsumeDeque(std::deque<SudokuBoard> &deque, int dest, int tag, MPI_Comm comm, int count = -1);

/**
 * Send a SudokuBoard using MPI.
 *
 * @complexity - O(n) where n = number of cells in the board
 *
 * @param board - the board
 * @param dest - the destination process
 * @param tag - MPI_TAG
 * @param pCommunicator - MPI_COMM
 */
void sendSudokuBoard(SudokuBoard &board, int dest, int tag, MPI_Comm pCommunicator);

/**
 * Receive a SudokuBoard using MPI.
 *
 * @complexity - O(n) where n = number of cells in the board
 *
 * @param src - source process
 * @param tag - MPI_TAG
 * @param pCommunicator -  MPI_COMM
 */
SudokuBoard receiveSudokuBoard(int src, int tag, MPI_Comm pCommunicator);

/**
 * Use the master process as a load balancer between processes.
 * Each worker process will receive SudokuBoard to process on it side.
 * At the end, send back all results to master process.
 */
void initSolveMPI();


/**
 * Function used for tests
 */
void tests();

/**
 * Function used for tests
 */
void testsCrook();

/**
 * Function used for tests
 */
void testFromStdin();

#endif //INC_8INF856_PROJET_SUDOKU_H
