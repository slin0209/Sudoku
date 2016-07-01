CC=g++
CFLAGS=-I.
CFLAGS+=-std=c++11

DEPS = SudokuBoard.hpp
OBJ = SudokuBoard.o sudoku_main.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

sudoku: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)	

.PHONY: clean

clean:
	rm -f *.o 
