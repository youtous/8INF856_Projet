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

/**
 * How many sub-problems to generate on the master node ?
 */
static int COUNT_PROBLEMS_TO_GENERATE_ON_MASTER = 512;
/**
 * How many sub-problems to generate on the worker node when receiving work ?
 */
static int COUNT_PROBLEMS_TO_GENERATE_ON_WORKER = 512;

static int DEBUG = 0;

static const int DEBUG_BASE = 1;

int main(int argc, char *argv[]) {
    int processId;                              /* Process rank */
    int countProcess;                           /* Number of processes */

    if (argc >= 2) {
        DEBUG = std::atoi(argv[1]);
    }

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &countProcess);

    if (processId == 0 && DEBUG > DEBUG_BASE) {
        std::cout << "[" << processId << "]: DEBUG LEVEL = " << DEBUG << std::endl;
    }

#pragma omp parallel
    {
#pragma omp single
        {
            std::cout << "[" << processId << "]: Configured to use " << omp_get_num_threads() << " threads, "
                      << omp_get_num_procs() << " cores available."
                      //  <<std:endl<<  "Use 'export OMP_NUM_THREADS=8; mpirun -x OMP_NUM_THREADS ...' to define number of threads."
                      << std::endl;
        }
    }

    initSolveMPI();

    MPI_Finalize();
    return 0;
}

void initSolveMPI() {
    int processId;                              /* Process rank */
    int countProcess;                           /* Number of processes */
    std::deque<SudokuBoard> solutionBoards;     /* Sudoku solutions */

    // list of raw computed problems, it's a bruteforce computed
    // extrapolation of the initial board generated by solveBoard()
    std::deque<SudokuBoard> problemBoards;


    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &countProcess);

    // exec timing
    double p1Time = -MPI_Wtime();

    // init sudoku solving on master
    // compute first boards to investigate
    if (processId == 0) {
        SudokuBoard sudoku = createFromStdin();

        std::cout << "[" << processId << "]: SudokuBoard(" << sudoku.getSudokuDimension() << ") "
                  << sudoku.getColumnSize() << "x" << sudoku.getRowSize() << " to solve : " << std::endl << sudoku
                  << std::endl;

        // generate the first sub-problems in order to dispatch work between nodes
        problemBoards.emplace_front(std::move(sudoku));
        while (!problemBoards.empty() && problemBoards.size() < COUNT_PROBLEMS_TO_GENERATE_ON_MASTER) {
            SudokuBoard solution = generatePossibilitiesNextCell(problemBoards);

            if (!solution.isEmpty()) {
                // solution found, end of generation
                solutionBoards.emplace_back(std::move(solution));
                break;
            }
        }

        if (!solutionBoards.empty()) {
            std::cout << "[" << processId << "]: Solution found during first generation :" << std::endl
                      << solutionBoards.front()
                      << std::endl;
            std::deque<SudokuBoard> empty;
            std::swap(problemBoards, empty);
        }

        std::cout << "[" << processId << "]: Generated " << problemBoards.size()
                  << " initial problem boards to dispatch between workers."
                  << std::endl;
    }

    // the worker who found the solution
    int firstWinnerWorker = -1;
    // balance load dynamically between processes
    int successWorkerId = -1;
    if (processId == 0) {
        MPI_Status idleRequestStatus;
        // master process opens idle requests from workers
        std::vector<MPI_Request> workersRequests(countProcess - 1);
        std::vector<int> countSolutionsFoundOnProcess(countProcess - 1, 0);
        for (int workerId = 1; workerId < countProcess; ++workerId) {
            MPI_Irecv(countSolutionsFoundOnProcess.data() + workerId - 1, 1, MPI_INT, workerId, CUSTOM_MPI_IDLE_TAG,
                      MPI_COMM_WORLD, (workersRequests.data() + workerId - 1));
        }

        // distribute work, iterate over each process worker until no work left
        int idleResponse;
        const int initialProblemsSize = problemBoards.size();
        while (!problemBoards.empty()) {
            for (int workerId = 1; workerId < countProcess; ++workerId) {
                idleResponse = 0;
                // check if the request has been completed
                MPI_Test((workersRequests.data() + workerId - 1), &idleResponse, &idleRequestStatus);

                // worker is idle ! send it some work
                if (idleResponse) {
                    // check if the worker has finished ?
                    if (countSolutionsFoundOnProcess[workerId - 1] > 0) {
                        // a worker has found a solution, remove remaining problem boards
                        successWorkerId = workerId;
                        std::cout << "[" << processId << "]: " << workerId << " just found a solution !" << std::endl;
                        // empty working queue then finish
                        std::deque<SudokuBoard> empty;
                        std::swap(problemBoards, empty);
                        break;
                    } else {
                        // otherwise, send it some work
                        if (DEBUG >= DEBUG_BASE) {
                            std::cout << "[" << processId << "]: sending 1 problem board to process[" << workerId << "]"
                                      << std::endl;
                        }
                        std::cout << "\r[" << processId << "]: Dispatching ";
                        std::cout << initialProblemsSize - problemBoards.size() << "/" << initialProblemsSize
                                  << std::flush;
                        std::cout << " problems boards between workers.";

                        sendAndConsumeDeque(problemBoards, workerId, CUSTOM_MPI_POSSIBILITIES_TAG, MPI_COMM_WORLD, 1);
                        MPI_Irecv(countSolutionsFoundOnProcess.data() + workerId - 1, 1, MPI_INT, workerId,
                                  CUSTOM_MPI_IDLE_TAG, MPI_COMM_WORLD,
                                  (workersRequests.data() + workerId - 1));
                    }
                }
            }
        }
        std::cout << "... finished!" << std::endl;

        if (DEBUG >= DEBUG_BASE) {
            std::cout << "[" << processId << "]: all problem boards have been computed!" << std::endl;
        }
    } else {
        unsigned int processLoad = 0;
        // workers wait for work to do while the working queue is not empty
        MPI_Request workerRequestIdle;
        MPI_Status idleRequestStatus;
        do {
            // notice master process is idle
            // send to master the number of solutions founds
            int countSolutions = solutionBoards.size();
            MPI_Isend(&countSolutions, 1, MPI_INT, 0, CUSTOM_MPI_IDLE_TAG, MPI_COMM_WORLD, &workerRequestIdle);
            MPI_Wait(&workerRequestIdle, &idleRequestStatus);

            // wait work from master
            int countReceivedBoards = receivePushBackDeque(problemBoards, 0, CUSTOM_MPI_POSSIBILITIES_TAG,
                                                           MPI_COMM_WORLD);
            processLoad += countReceivedBoards;

            if (countReceivedBoards > 0) {
                SudokuBoard solution = solveProblemsOnNode(problemBoards);
                if (!solution.isEmpty()) {
                    solutionBoards.emplace_back(solution);
                    firstWinnerWorker = processId;
                    if (DEBUG >= DEBUG_BASE) {
                        std::cout << "[" << processId << "]: a solution has been found :" << std::endl
                                  << solution << std::endl;
                    }
                    // mark current process as winner and inform master process, then finish
                    successWorkerId = processId;
                    countSolutions = solutionBoards.size();
                    MPI_Isend(&countSolutions, 1, MPI_INT, 0, CUSTOM_MPI_IDLE_TAG, MPI_COMM_WORLD, &workerRequestIdle);
                    break;
                }
            } else {
                // end of the work, no more board to compute
                break;
            }
        } while (true);

        std::cout << "[" << processId << "]: finished to work. " << solutionBoards.size()
                  << " solutions found over "
                  << processLoad << " problem boards assigned." << std::endl;
    }

    // collect results
    if (processId == 0) {
        if (successWorkerId != -1) {
            receivePushBackDeque(solutionBoards, successWorkerId, CUSTOM_MPI_SOLUTIONS_TAG,
                                 MPI_COMM_WORLD);
            std::cout << "[" << processId << "]: Worker[" << successWorkerId << "] found a solution." << std::endl;
            std::cout << "[" << processId << "] Solution for board:" << std::endl << solutionBoards.front()
                      << std::endl;
        } else {
            std::cout << "[" << processId << "] No solution found for the board." << std::endl;
        }
    }
    if (successWorkerId == processId) {
        // send results to master
        if (processId == firstWinnerWorker) {
            sendAndConsumeDeque(solutionBoards, 0, CUSTOM_MPI_SOLUTIONS_TAG, MPI_COMM_WORLD);
        }
    }

    // finish timing
    p1Time += MPI_Wtime();
    if (processId == 0) {
        std::cout << std::fixed;
        std::cout << "Process [" << processId << "] : sudoku solving puzzle " << " on " << countProcess
                  << " processes took: "
                  << p1Time << " seconds. " << std::endl;
        std::cout.unsetf(std::ios::fixed);
    }
    if (processId == 0) {
        // end of the work for everyone !
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
}

// Begin of Solver methods
SudokuBoard solveBoardRecursive(SudokuBoard &board, bool &solutionFound) {
    board.recountSolvedCells();
    board.computePossiblesValuesInCells();
    return solveBoard(board, solutionFound);
}

SudokuBoard solveBoard(SudokuBoard &board, bool &solutionFound, int row, int col) {
    if (solutionFound) {
        return SudokuBoard(0);
    }
    // current cell computed
    const int index = row * board.getRowSize() + col;

    // check end reached => terminate recursion
    if (index >= board.getSize()) {
        // the board is solved, return it
        return board;
    }

    // apply humanistic heuristic
    bool changedElimination = false;
    bool changedLoneRangers = false;
    bool changedTwins = false;
    bool changedTriplets = false;
    do {
        changedElimination = eliminatationStrategy(board);
        if (board.isSolved()) {
            return board;
        }
        if (changedElimination) {
            continue;
        }

        changedLoneRangers = lonerangerStrategy(board);
        if (board.isSolved()) {
            return board;
        }
        if (changedLoneRangers) {
            continue;
        }

        changedTwins = twinsStrategy(board);
        if (board.isSolved()) {
            return board;
        }
        if (changedTwins) {
            continue;
        }

        changedTriplets = tripletsStrategy(board);
        if (board.isSolved()) {
            return board;
        }
    } while (changedElimination || changedLoneRangers || changedTwins || changedTriplets);


    // next cell values
    bool jumpRow = (col + 1) >= board.getRowSize();
    int nextRow = row + (jumpRow ? 1 : 0);
    int nextCol = jumpRow ? 0 : col + 1;

    // add the current value of the cell in order to propagate
    if (board[row][col] != 0) {
        board.addPossibleValueForCell(row, col, board[row][col]);
    }

    // cell not solved, try all possible numbers in the cell
    // value is valid, continue in to deep search
    for (int possibleValue :board.getPossiblesValuesInCells()[row][col]) {
        SudokuBoard copyBoard = board;

        // set the value
        copyBoard.setValueAndUpdatePossibilities(row, col, possibleValue);

        SudokuBoard solution = solveBoard(copyBoard, solutionFound, nextRow, nextCol);
        // if solution has been found, return recursion
        if (!solution.isEmpty()) {
            return solution;
        }
    }

    return SudokuBoard(0);
}

SudokuBoard generatePossibilitiesNextCell(std::deque<SudokuBoard> &boardsToWork) {
    if (boardsToWork.empty()) {
        // no solution remaining
        throw std::invalid_argument("Given boards to compute is empty, no reduction can be done.");
    }

    SudokuBoard &workingBoard = boardsToWork.front();

    auto nextEmptyCell = workingBoard.nextEmptyCell();

    // all cells have a value, we found a solution,
    // add it to the solutions list, JOB IS DONE !
    if (nextEmptyCell.first == -1) {
        SudokuBoard solution = SudokuBoard(workingBoard);
        boardsToWork.pop_front();
        return solution;
    }

    // possibles values for the cell
    std::deque<int> possibleValuesInCell;

    // check if the current board can be completed
    // we use bruteforce in order to check if we are not dealing with a dead leaf
    // of the tree
    for (int i = 1; i <= workingBoard.getBlockSize(); ++i) {
        if (workingBoard.testValueInCell(nextEmptyCell.first, nextEmptyCell.second, i)) {
            possibleValuesInCell.push_back(i);
        }
    }

    if (possibleValuesInCell.empty()) {
        // no solution for this board, next!
        boardsToWork.pop_front();
        return SudokuBoard(0);
    }

    // create a new board to check for each possible value
    // and add it to the work queue
    for (auto const &value: possibleValuesInCell) {
        workingBoard[nextEmptyCell.first][nextEmptyCell.second] = value;
        boardsToWork.emplace_back(workingBoard);
        possibleValuesInCell.pop_front();
    }

    boardsToWork.pop_front();

    return SudokuBoard(0);
}

SudokuBoard solveProblemsOnNode(std::deque<SudokuBoard> &problems) {
    int processId;                              /* Process rank */
    int countProcess;                           /* Number of processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);
    MPI_Comm_size(MPI_COMM_WORLD, &countProcess);

    // generate sub-problems in order to dispatch work between threads
    while (!problems.empty() && problems.size() < COUNT_PROBLEMS_TO_GENERATE_ON_WORKER) {
        SudokuBoard solution = generatePossibilitiesNextCell(problems);

        if (!solution.isEmpty()) {
            // solution found during generation
            return solution;
        }
    }

    if (DEBUG >= DEBUG_BASE) {
        std::cout << "[" << processId << "]: generated " << problems.size() << " problem boards to check." << std::endl;
    }

    bool solutionFound = false;
    std::deque<SudokuBoard> solutions;
// creating tasks pool
#pragma omp parallel shared(solutionFound, solutions, problems)
    {
#pragma omp single nowait
        {
            for (int i = 0; i < problems.size(); ++i) {
#pragma omp task
                {
                    if (!solutionFound) {
                        // we can't break loop :( beurk OpenMP 3, vive OpenMP 5

                        if (DEBUG >= DEBUG_BASE) {
                            std::cout << "[" << processId << "]{" << omp_get_thread_num()
                                      << "}: solving a board on problem over " << omp_get_num_threads()
                                      << " threads." << std::endl;
                        }

                        // update existing values
                        SudokuBoard solution = solveBoardRecursive(problems[i], solutionFound);

                        if (!solution.isEmpty()) {
#pragma omp critical
                            {
                                // see : http://jakascorner.com/blog/2016/08/omp-cancel.html
                                solutionFound = true;
                                solutions.emplace_back(std::move(solution));
                            }
                            std::cout << "[" << processId << "]{" << omp_get_thread_num()
                                      << "}: found a solution, the programm should stop." << std::endl
                                      << "Problem equality : " << (solutions.front() == problems[i]) << std::endl;
                        }
                    }
                }
            }
        }
    }

    // clear problems
    problems.clear();

    if (!solutions.empty()) {
        std::cout << "[" << processId << "]{" << omp_get_thread_num() << "}: found a solution, returning..."
                  << std::endl;
        return solutions.front();
    }

    // no solution found
    return SudokuBoard(0);
}

void SudokuBoard::addPossibleValueForCell(int row, int col, int value) {
    if (value < 1 || value > this->getBlockSize()) {
        std::stringstream ss;
        ss << "Could not add " << value << " to cell {" << row << "," << col << "}. Value must be between 1 and "
           << getBlockSize() << ".";
        throw std::invalid_argument(ss.str());
    }
    this->getPossiblesValuesInCells()[row][col].insert(value);
    this->getPossiblesValuesInRows()[row].insert(value);
    this->getPossiblesValuesInColumns()[col].insert(value);
    this->getPossiblesValuesInBlocks()[this->getBlockOfCell(row, col)].insert(value);

}

void SudokuBoard::setValueAndUpdatePossibilities(int row, int col, int value) {
    this->operator[](row)[col] = value;
    this->getPossiblesValuesInCells()[row][col].clear();
    this->getPossiblesValuesInRows()[row].erase(value);
    this->getPossiblesValuesInColumns()[col].erase(value);
    this->getPossiblesValuesInBlocks()[this->getBlockOfCell(row, col)].erase(value);
}

bool SudokuBoard::testValueInCellFromCompute(int row, int col, int value) const {
    // same value
    if (this->get(row, col) == value) {
        return true;
    }
    // value already exists
    if (this->get(row, col) != 0) {
        return false;
    }

    if (possiblesValuesInRows[row].find(value) == possiblesValuesInRows[row].end()) {
        // not found
        return false;
    }
    if (possiblesValuesInColumns[col].find(value) == possiblesValuesInColumns[col].end()) {
        // not found
        return false;
    }
    if (possiblesValuesInBlocks[getBlockOfCell(row, col)].find(value) ==
        possiblesValuesInBlocks[getBlockOfCell(row, col)].end()) {
        // not found
        return false;
    }
    return true;
}

bool SudokuBoard::testValueInCell(int row, int col, int value) const {
    // same value
    if (this->get(row, col) == value) {
        return true;
    }
    // value already exists
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

bool eliminatationStrategy(SudokuBoard &board) {
    return false;
    int solvedCells = board.getCountSolvedCells();
    for (int row = 0; row < board.getColumnSize(); ++row) {
        for (int col = 0; col < board.getRowSize(); ++col) {
            if (board[row][col] == 0 &&
                board.getPossiblesValuesInCells()[row][col].size() == 1) {
                // todo : fix this
                // board[row][col] = board.getPossiblesValuesInCells()[row][col][0];
                solvedCells += 1;
            }
            if (solvedCells == board.getSize()) {
                break;
            }
        }
        if (solvedCells == board.getSize()) {
            break;
        }
    }

    bool changed = board.getCountSolvedCells() != solvedCells;
    board.setCountSolvedCells(solvedCells);
    return changed;
}

bool lonerangerStrategy(SudokuBoard &board) {
    return false;
}

bool twinsStrategy(SudokuBoard &board) {
    return false;
}

bool tripletsStrategy(SudokuBoard &board) {
    return false;
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
    /**if (row > this->rows - 1 || row < 0 || col > this->cols - 1 || col < 0) {
        std::stringstream ss;
        ss << "Trying to get [" << row << "," << col << "] on a [" << rows << "," << cols << "] matrix";
        throw std::out_of_range(ss.str());
    } */
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

std::pair<int, int> SudokuBoard::nextEmptyCell() const {
    for (int row = 0; row < this->getColumnSize(); row++) {
        for (int col = 0; col < this->getRowSize(); col++) {
            if (this->get(row, col) == 0) {
                return {row, col};
            }
        }
    }

    return {-1, -1};
}

int SudokuBoard::getBlockOfCell(int row, int col) const {
    return getBlockRowOf(row) * this->n + getBlockColOf(col);
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

std::vector<std::vector<std::set<int>>> &SudokuBoard::getPossiblesValuesInCells() {
    return this->possiblesValuesInCells;
}

std::vector<std::vector<std::set<int>>> const &SudokuBoard::getPossiblesValuesInCells() const {
    return this->possiblesValuesInCells;
}

std::vector<std::set<int>> const &SudokuBoard::getPossiblesValuesInRows() const {
    return this->possiblesValuesInRows;
}

std::vector<std::set<int>> &SudokuBoard::getPossiblesValuesInRows() {
    return this->possiblesValuesInRows;
}

std::vector<std::set<int>> const &SudokuBoard::getPossiblesValuesInColumns() const {
    return this->possiblesValuesInColumns;
}

std::vector<std::set<int>> &SudokuBoard::getPossiblesValuesInColumns() {
    return this->possiblesValuesInColumns;

}

std::vector<std::set<int>> const &SudokuBoard::getPossiblesValuesInBlocks() const {
    return this->possiblesValuesInBlocks;
}

std::vector<std::set<int>> &SudokuBoard::getPossiblesValuesInBlocks() {
    return this->possiblesValuesInBlocks;
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
int SudokuBoard::recountSolvedCells() {
    this->countSolvedCells = 0;
    for (auto const &e: arrAsLine) {
        if (e != 0) {
            this->countSolvedCells += 1;
        }
    }
    return this->countSolvedCells;
}

void SudokuBoard::computePossiblesValuesInCells() {
    this->possiblesValuesInCells.clear();

    std::set<int> possiblesValues;
    for (int i = 1; i <= this->getRowSize(); ++i) {
        possiblesValues.insert(i);
    }

    this->possiblesValuesInColumns.resize(countColumns(), possiblesValues);
    this->possiblesValuesInRows.resize(countRows(), possiblesValues);
    this->possiblesValuesInBlocks.resize(countBlocks(), possiblesValues);
    this->possiblesValuesInCells.resize(countRows(), std::vector<std::set<int>>(countColumns()));

    for (int row = 0; row < countRows(); ++row) {
        for (int col = 0; col < countColumns(); ++col) {
            const int value = this->get(row, col);
            // value already set
            if (value != 0) {
                possiblesValuesInCells[row][col].clear();
                possiblesValuesInBlocks[getBlockOfCell(row, col)].erase(value);
                possiblesValuesInRows[row].erase(value);
                possiblesValuesInColumns[col].erase(value);
            } else {
                // test with values
                for (int valueTest = 1; valueTest <= getBlockSize(); ++valueTest) {
                    if (testValueInCell(row, col, valueTest)) {
                        this->possiblesValuesInCells[row][col].insert(valueTest);
                    }
                }
            }
        }
    }
}

bool SudokuBoard::isSolved() const { return this->getCountSolvedCells() == this->getSize(); }

int SudokuBoard::getCountSolvedCells() const {
    return countSolvedCells;
}

void SudokuBoard::setCountSolvedCells(int countSolvedCells) {
    this->countSolvedCells = countSolvedCells;
}

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

bool SudokuBoard::operator==(const SudokuBoard &rhs) const {
    return arrAsLine == rhs.arrAsLine &&
           n == rhs.n &&
           rows == rhs.rows &&
           cols == rhs.cols;
}

bool SudokuBoard::operator!=(const SudokuBoard &rhs) const {
    return !(rhs == *this);
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


// Begin of MPI exchanges methods
int receivePushBackDeque(std::deque<SudokuBoard> &dequeue, int src, int tag, MPI_Comm comm) {
    unsigned int len;
    MPI_Status s;
    MPI_Recv(&len, 1, MPI_UNSIGNED, src, tag, comm, &s);

    for (int i = 0; i < len; ++i) {
        dequeue.emplace_back(receiveSudokuBoard(src, tag, comm));
    }
    return len;
}

void sendAndConsumeDeque(std::deque<SudokuBoard> &deque, int dest, int tag, MPI_Comm comm, int count) {
    unsigned int len = count == -1 ? deque.size() : std::min((int) deque.size(), count);
    MPI_Send(&len, 1, MPI_UNSIGNED, dest, tag, comm);

    for (int i = 0; i < len; ++i) {
        sendSudokuBoard(deque.front(), dest, tag, comm);
        deque.pop_front();
    }
}


void sendSudokuBoard(SudokuBoard &board, int dest, int tag, MPI_Comm pCommunicator) {
    int sudokuSize = board.getSudokuDimension();
    MPI_Send(&sudokuSize, 1, MPI_INT, dest, tag, pCommunicator);
    if (sudokuSize != 0) {
        MPI_Send(board.data(), board.getSize(), MPI_INT, dest, tag, pCommunicator);
    }
}


SudokuBoard receiveSudokuBoard(int src, int tag, MPI_Comm pCommunicator) {
    int sudokuSize;
    MPI_Status s;
    MPI_Recv(&sudokuSize, 1, MPI_INT, src, tag, pCommunicator, &s);
    SudokuBoard board(sudokuSize);
    if (sudokuSize != 0) {
        MPI_Recv(board.data(), board.getSize(), MPI_INT, src, tag, pCommunicator, &s);
    }
    return board;
}
// End of MPI exchanges methods


// Begin of test methods
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
// End of test methods

