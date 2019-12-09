cmake_minimum_required(VERSION 3.15)
project(8INF856_Projet)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(8INF856_Projet sudoku.cpp)
add_executable(SudokuGenerator generator.cpp)
