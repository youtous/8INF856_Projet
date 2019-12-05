//
// Created by youtous on 30/11/2019.
//

#ifndef INC_8INF856_PROJET_SUDOKU_H
#define INC_8INF856_PROJET_SUDOKU_H


#include <vector>
#include <cmath>
#include <stdexcept>

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
     * @return - size of a row
     */
    inline int getRowSize() { return rows; }

    /**
     * @complexity - O(1) - constant
     * @return - size of a column
     */
    inline int getColumnSize() { return cols; }

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
     * SudokuRow represents a row of the Matrix.
     */
    class SudokuRow {

        /**
         * Mama <3
         */
        friend class Sudoku;

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
 * @param fileName - file containing SudokuBoard's data.
 * @return - SudokuBoard initiated from the File.
 */
SudokuBoard createFromFile(std::string const &fileName);

/**
 * Create a SudokuBoard from an stdin, the first element contains
 * the size of the SudokuBoard.
 * @return - SudokuBoard initiated from the File.
 */
SudokuBoard createFromStdin();

/**
 * Write in a File a given content.
 * @param fileName - path of the file to write.
 * @param contentFile - content of the file to set.
 */
void writeInFile(std::string const &fileName, std::string const &contentFile);


/**
 * Function used for tests
 */
void tests();

/**
 * Function used for tests
 */
void testFromStdin();
void solve();

#endif //INC_8INF856_PROJET_SUDOKU_H
