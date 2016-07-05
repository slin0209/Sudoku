CC=g++
CPPFLAGS=-I.
CPPFLAGS+=-std=c++11

DEPS = SudokuBoard.hpp
OBJ = SudokuBoard.o sudoku_main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CPPFLAGS)

sudoku: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS)	

.PHONY: clean

clean:
	rm -f *.o 
