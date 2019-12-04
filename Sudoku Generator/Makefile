CC=g++
CFLAGS=-std=c++1z
LDFLAGS=-O3 -c

# Execute Sudoku generator
sudoku-gen: main.o sudoku-gen.o
	$(CC) $(CFLAGS) $? -o $@

# Build all
%.o: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean: ## Clean the project
	rm -f *.o

