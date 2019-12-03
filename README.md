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
- Sudoku illustrated example with Constraints : https://pdfs.semanticscholar.org/1055/9030f571f05d18831edead72281d48f78752.pdf
- Sudoku illstrated with Deep First : https://github.com/Shivanshu-Gupta/Parallel-Sudoku-Solver
- Paper with deeper Sudoku exploration : https://shawnjzlee.me/dl/efficient-parallel-sudoku.pdf
- Sudoku deeper presentation : individual.utoronto.ca/rafatrashid/Projects/2012/SudokuPresentation.pptx
- How to generate a grid : https://stackoverflow.com/questions/4066075/proper-data-structure-to-represent-a-sudoku-puzzle


========================
- Best essay about sudoku solving problem : http://norvig.com/sudoku.html
- Related implementation in C++: https://github.com/pauek/norvig-sudoku/blob/master/sudoku.en.cc