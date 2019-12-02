#ifndef INC_8INF856_PROJET_GRID_H
#define INC_8INF856_PROJET_GRID_H

/**
 * @file grid.h
 * @brief The header file of the grid class
 * @authors Yann Galan, Léo-Gad Journel
 * @version 0.1
 * @date 02-12-2019
 *
 * This class allows the creation of a squared grid for the Sudoku game.
 */

/**
 * @class Grid
 *
 * This class represent a sudoku grid.
 */
class grid {
private:
    int gridSize;   /*!< The size of the grid */

public:
    /**
     * The grid constructor
     *
     * @param gridSize THe size of the grid
     */
    grid(int gridSize);

    /**
     * The grid destructor
     */
    virtual ~grid();

    /**
     * Method to create a new grid with a defined size
     *
     * @param gridSize The size of the grid
     */
    void create(int gridSize);

    /**
     * Method to get the size of the grid
     *
     * @return The size of the grid
     */
    int getGridSize() const { return gridSize; };
};


#endif //INC_8INF856_PROJET_GRID_H
