# 8INF856_Projet

## Commands :

- Use the following combinaison : `make clean && make sudoku-cluster && make sudoku-cluster-exec`

- `make sync` :              synchronize files on every nodes
- `make sudoku-cluster` :      compile then synchronize sudoku on every nodes
- `make sudoku-cluster-exec` : execute sudoku on every nodes (use `make sudoku-cluster` before)
- `make d3-2-cluster` :      compile then synchronize d3-2 on every nodes
- `make d3-2-cluster-exec` : execute d3-2 on every nodes (use `make d3-2-cluster` before)
- `make help` :              show help

## References:
- SudokuBoard illustrated example with Constraints : https://pdfs.semanticscholar.org/1055/9030f571f05d18831edead72281d48f78752.pdf
- SudokuBoard illstrated with Deep First : https://github.com/Shivanshu-Gupta/Parallel-SudokuBoard-Solver
- Paper with deeper SudokuBoard exploration : https://shawnjzlee.me/dl/efficient-parallel-sudoku.pdf
- SudokuBoard deeper presentation : https://individual.utoronto.ca/rafatrashid/Projects/2012/SudokuPresentation.pptx

- (nice) https://cse.buffalo.edu/faculty/miller/Courses/CSE633/Sankar-Spring-2014-CSE633.pdf
- (nice) Humanistic vs Bruteforce solver ; https://www.andrew.cmu.edu/user/astian/
- (nice) Big O https://stackoverflow.com/questions/15327376/algorithm-complexity-big-o-of-sudoku-solver
- Crook algorithm : https://github.com/maijoshi/ParaSudoku/blob/master/crook.cpp
- Bruteforce implementation : https://github.com/hecrj/par/blob/master/lab4/sudoku.c

todo : check this https://github.com/pranjalmaheshwari/sudokuSolver/blob/master/sudoku.c, 
todo : https://github.com/bryanesmith/SudokuBoard-solver/blob/master/SudokuPuzzle.cpp#L164

========================
- Best essay about sudoku solving problem : http://norvig.com/sudoku.html
- Related implementation in C++: https://github.com/pauek/norvig-sudoku/blob/master/sudoku.en.cc

```
Sudoku 3 nbBlanks = 81/2	possibilities=9     O = 33
Sudoku 4 nbBlanks = 256/2	possibilities=16    O = 144
Sudoku 5 nbBlanks = 625/2	possibilities=25    O = 289
Sudoku 6 nbBlanks = 1296/2	possibilities=36    O = 684

O(n ^ m) where n is the number of possibilities for each square (i.e., 9 in classic Sudoku) and m is the number of spaces that are blank.
```