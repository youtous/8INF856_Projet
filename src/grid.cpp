/**
 * @file grid.cpp
 * @brief The source file of the grid class
 * @authors Yann Galan, Léo-Gad Journel
 * @version 0.1
 * @date 02-12-2019
 *
 * This class allows the creation of a squared grid for the Sudoku game.
 */

#include "grid.h"

grid::grid(int gridSize) : gridSize(gridSize) {
    create(gridSize);
}

grid::~grid() = default;

void grid::create(int gridSize) {

}

