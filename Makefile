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
	mpiexec --npernode 1 -n 16 $< 0 < grid_reference_4_3.txt

sync: ## Synchronise files on cluster
	chmod +x ./sync.sh
	./sync.sh

clean: ## Clean the project
	rm -f *.o

help: ## Show this help prompt.
