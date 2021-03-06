#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/time.h>

#include "SudokuBoard.hpp"

bool debug_on = false;

void usage(char* name)
{
	std::cout<<"Usage: "<<name<<"  <input file>  [debug]"<<std::endl;
}

int main(int argc, char* argv[] )
{
	if(argc == 1 || argc > 3)
	{
		usage(argv[0]);
		return -1;
	}

	if(argc == 3)
	{
		if(strcmp("debug",argv[2]) == 0 )
			debug_on = true;
		else 
		{
			usage(argv[0]);
			return -1;
		}
	}
	
	std::ifstream inputfile (argv[1]);
	int array[BOARD_LINES][BOARD_COLUMN];

	int i = 0, j = 0;
	
	char c = inputfile.get();
	while(inputfile.good()) 
	{
		if(c == '\n') 
		{
			i++;
			j = 0;
		}else if(c >= '0' && c <= '9' )
		{
			array[i][j] = atoi(&c);
			j++;
		}
		c = inputfile.get();
	}	
	inputfile.close();

	std::cout <<"Start to create sudokuboard!"<<std::endl;
	struct timeval start, stop;
	gettimeofday(&start, NULL);
	
	SudokuBoard sudoku_board(array);
	int ret = sudoku_board.run();
	
	gettimeofday(&stop, NULL);
	int msecs = (stop.tv_sec - start.tv_sec) * 1000 + (stop.tv_usec - start.tv_usec)/1000;
	
	if(ret == 0)
	{
		std::cout<<"We get a solution!"<<std::endl;
		sudoku_board.output_result();
		std::cout<<"It took me "<<msecs<<" milliseconds to finish!"<<std::endl;
		return 0;
	}else
	{
		std::cout<<"We can't find a solution for this configuration!"<<std::endl;
		return -1;
	}
}
