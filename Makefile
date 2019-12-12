#!make
.DEFAULT_GOAL=help
.PHONY=sync

CC=mpic++
CFLAGS=-std=c++11 -fopenmp
LDFLAGS=

sudoku.o: sudoku.cpp ## Compile sequential app
	$(CC) $< -o $@ $(CFLAGS)

sudoku: sudoku.o ## Execute app
	chmod +x $<
	./$< grille.txt

sudoku-cluster: sudoku.o ## Compile then sync sudoku on the cluster
	chmod +x $<
	./sync.sh

sudoku-cluster-exec: sudoku.o  ## Execute sudoku on the cluster
	export OMP_NUM_THREADS=16; mpirun -x OMP_NUM_THREADS --npernode 1 -np 16 $< 0 < puzzles_reference/5.txt

generator.o: generator.cpp ## Compile sequential app
	$(CC) $< -o $@ $(CFLAGS)


generator.o: generator.cpp ## Compile sequential app
	$(CC) $< -o $@ $(CFLAGS)

sync: ## Synchronise files on cluster
	chmod +x ./sync.sh
	./sync.sh

clean: ## Clean the project
	rm -f *.o

help: ## Show this help prompt.
