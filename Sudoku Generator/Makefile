run-native: sudoku-gen
	./sudoku-gen 100

# build linux binary
sudoku-gen: main.o sudoku-gen.o
	g++ -std=c++1z $? -o sudoku-gen

# build all object files for linux
%.o: %.cpp
	g++ -std=c++1z -O3 -c $< -o $@

clean: ## Clean the project
	rm -f *.o