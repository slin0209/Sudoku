#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <stack>

#include "SudokuBoard.hpp"

extern bool debug_on;

SudokuBoard::SudokuBoard(int array[][BOARD_COLUMN])
{
	blanks = BOARD_LINES * BOARD_COLUMN;
	if(debug_on)
		std::cout<< "Start constructor of SudokuBoard!" <<std::endl;	
	
	for(int i = 0; i < BOARD_LINES; i++)
	{
		for(int j = 0; j < BOARD_COLUMN; j++)
		{
			if(array[i][j] != 0)
			{
				blanks--;
			}
			
			spot_array[i][j].setup(array[i][j], i, j); 
		}
	}

	if(debug_on)
		std::cout<< "Finish constructor of SudokuBoard!" <<std::endl;		
}

void SudokuBoard::prepare()
{
	block_type type[] = { BLOCK, VER_LINE, HOR_LINE};

	if(debug_on)
		std::cout <<"In prepare..." <<std::endl;
	//Go through the block, initialize the value of block_line appropriate.
	for(int i = 0; i < BLOCK_TYPES;i++)
	{
		for(int j = 0; j < BLOCK_NUM;j++)
		{
			bl_array[i][j].setup(type[i], j);
			//Calculate empty values.
			if(type[i] == BLOCK) 
			{
				for(int k = 0;k < BLOCK_ENTRIES;k++ )
				{
					if(spot_array[j/3*3+k/3][j%3*3+k%3].show_val() != 0)
						 bl_array[i][j].update_bitmap(k, SET);
				}
			}
			else if(type[i] == VER_LINE)
			{
				for(int k = 0;k < BLOCK_ENTRIES;k++ )
				{
					if(spot_array[k][j].show_val() != 0)
						bl_array[i][j].update_bitmap(k, SET);
				}
			}
			else
			{
				for(int k = 0;k < BLOCK_ENTRIES;k++ )
				{
					if(spot_array[j][k].show_val() != 0)
						bl_array[i][j].update_bitmap(k, SET);
				}			
			}
			
			block_vec.push_back(&bl_array[i][j]);
		}
	}

	std::sort(block_vec.begin(), block_vec.end(), compare());

	//Iinitialize potential values of every spot
	for(int i = 0; i < BOARD_LINES;i++)
	{
		for( int j = 0;j < BOARD_COLUMN;j++)
		{
			if(spot_array[i][j].show_val() != 0)
			{
				spot_array[i][j].clear_bitmap();				
			}else
			{
				//calculate the potential values of this element
				for(int k = 0;k < BOARD_COLUMN;k++) //go through elements on the same line 
				{
					if(k == j)
						continue;
					
					int bit;
					if( (bit = spot_array[i][k].show_val()) != 0)
					{
						spot_array[i][j].update_bitmap(bit, SET, HOR_LINE);
					}
				}

				for(int k = 0; k < BOARD_LINES;k++)
				{
					if(k == i)
						continue;

					int bit;
					if((bit = spot_array[k][j].show_val()) != 0)
					{
						spot_array[i][j].update_bitmap(bit, SET, VER_LINE);					
					}
				}

				//the block index of this spot is i/3*3+j/3
				int index = i/3*3 + j/3;
				for(int k = 0; k <BLOCK_ENTRIES;k++)
				{
					int x = index/3*3+k/3;
					int y = index%3*3+k%3;
					
					if(x == i && y == j)
						continue;

					int bit;
					if((bit = spot_array[x][y].show_val()) != 0)
					{
						spot_array[i][j].update_bitmap(bit, SET, BLOCK);					
					}
				}
			}
		}
	}
	
	if(debug_on)	
		std::cout<< "Finish prepare" <<std::endl;
}

SudokuBoard::spot& SudokuBoard::pick_spot_from_block()
{
	std::vector<block_line*>::iterator it;

	for(it = block_vec.begin();it != block_vec.end();it++)
	{
		block_line* block = *it;
		if( block->get_empty_vals() == 0)
		{
			continue;
		}else
		{
			//Pick one spot in the block
		 	int index = block->get_index();
		 	int type = block->type();

			if(debug_on)
				std::cout<<"Pick block type="<<type<<", index="<<index<<" to find a spot"<<std::endl;
			
			if(type == HOR_LINE) 
			{
				for(int i = 0;i < BLOCK_ENTRIES;i++)
				{
					if(spot_array[index][i].show_val() == 0)
					{
						if(debug_on)
						{
							std::cout<<"We pick spot index("<<index<<","<<i<<")"<<std::endl;
							spot_array[index][i].display();
						}
						return spot_array[index][i];
					}
				}
			}else if(type == VER_LINE)
			{
				for(int i = 0;i < BLOCK_ENTRIES;i++)
				{
					if(spot_array[i][index].show_val() == 0)
					{
						if(debug_on)
						{
							std::cout<<"We pick spot index("<<i<<","<<index<<")"<<std::endl;
							spot_array[i][index].display();					
						}
						return spot_array[i][index];
					}
				}			
			}else   /* type == BLOCK */
			{
				for(int i = 0; i < BLOCK_ENTRIES;i++)
				{
					int x = index/3*3+i/3;
					int y = index%3*3+i%3;

					if(spot_array[x][y].show_val() == 0)
					{
						if(debug_on)
						{
							std::cout<<"We pick spot index("<<x<<","<<y<<")"<<std::endl;
							spot_array[x][y].display();	
						}
						return spot_array[x][y];
					}					
				}			
			}	
		}
	}

	if(it == block_vec.end())
	{
		//I think we finished!
		//This should not happen, for we have blanks counter monitor
		//the number of spots left.We know which spot is the last one
		//just after it is set, and finish the process, there's no way
		//to get here, unless there is a bug.
	}
}

/*1.Update the potential of other spot in the same line
/*2.Update the potential of other spot in the same column
/*3.Update the potential of other spot in the same block
*/
void SudokuBoard::update_blocks(spot & spot, bitmap_action action)
{
	int x = spot.x;
	int y = spot.y;
	int val = spot.show_val();
	
	//update the block_line
	for(int i = 0;i < BLOCK_ENTRIES;i++)  // the same line
	{
		if(spot_array[x][i].show_val() == 0)
		{
			spot_array[x][i].update_bitmap(val, action, HOR_LINE);
		}
	}
	//update the block
	bl_array[HOR_LINE][x].update_bitmap(y, action);

	for(int i = 0;i < BLOCK_ENTRIES;i++) //the same colunm
	{
		if(spot_array[i][y].show_val() == 0)
		{
			spot_array[i][y].update_bitmap(val, action, VER_LINE);					
		}
	}

	bl_array[VER_LINE][y].update_bitmap(x, action);

	//the block index of this spot is i/3*3+j/3
	int index = x/3*3 + y/3;
	for(int i = 0; i < BLOCK_ENTRIES;i++)
	{
		int a = index/3*3+i/3;
		int b = index%3*3+i%3;

		if(spot_array[a][b].show_val() == 0)
		{
			spot_array[a][b].update_bitmap(val, action, BLOCK);					
		}					
	}
	//Calculate the entry index of spot(x,y) in the block.
	int a0 = index/3*3;
	int b0= index%3*3;
	int entry_idx = 3*(x - a0) + (y - b0);
	
	bl_array[BLOCK][index].update_bitmap(entry_idx, action);

	//re-sort the vector sequence, and then start again.
	std::sort(block_vec.begin(), block_vec.end(), compare());	

	if(debug_on)	
	{
		std::cout <<"After resort!" <<std::endl;
		std::vector<block_line*>::iterator it;
		block_line* block;
		std::cout << "List of blocks:" <<std::endl;
		for(it = block_vec.begin();it != block_vec.end();it++)
		{
			block = *it;
			block->display();
		}
	}
	return;
}

int SudokuBoard::run()   //This is where actually do the work.
{
	prepare();

	//OK, now do the real job
	//1.Pick a block to try
	//2.Guess a value to a spot, push the action in stack history
	//  and spot history
	//3.Update the sequence of the block_line, re-sort it.
	//   (We may find some more efficent way later on.)
	if(debug_on)
	{
		std::cout<<"Before we run, make sure everything is ok" << std::endl;
		std::vector<block_line*>::iterator it;
		block_line *block;
		std::cout << "List of blocks:" <<std::endl;
		for(it = block_vec.begin();it != block_vec.end();it++)
		{
			block = *it;
			block->display();
		}

		std::cout << "After preparation, List of spots:" <<std::endl;
		for(int i = 0; i< 9;i++)
		{
			for(int j = 0; j < 9;j++)
			{
				spot_array[i][j].display();
			}
		}
	}
	std::cout<< "We start to run now!" <<std::endl;
	spot* cur_spot = &pick_spot_from_block(); //Need to consider how to handle exceptions.
	
	while(1)
	{
		//1.Make a guess for this spot
		int val_pick = cur_spot->guess_val();
		//Check the value returned, we may be failed and need
		//to rollback.
		if(debug_on)
		{
			std::cout <<"Pick a new value=" <<val_pick<<", at  " 
				<<cur_spot->x <<"," <<cur_spot->y <<std::endl;
			output_result();
			std::cout <<"--------------------------" << std::endl;
		}
		
		if(val_pick == -1)
		{
			//We don't have potential value to choose.
			//1.Clear history of this spot.
			cur_spot->clear_history();
			
			//2.Pop the previous chose spot to consider other vlaues.
			if(stack.empty()) 
			{
				//We done, no solutions available for this Sudoku configuration.
				return -1;
			}
			
			cur_spot = stack.top();
			stack.pop();
			blanks++;
			
			//3.Reverse the last chose value.
			if(debug_on)
			{
				std::cout << "Reverse value=" <<cur_spot->show_val() <<"at"
					<<cur_spot->x <<"," <<cur_spot->y <<std::endl;
			}
			
			update_blocks(*cur_spot, CLEAR);
			cur_spot->clear_val();
			
			if(debug_on)
				cur_spot->display();
			
			continue;
		}

		//We have picked one value, save the history
		stack.push(cur_spot);
		blanks--;
		if(blanks == 0)
		{
			//we success!
			return 0; 
		}
		
		if(debug_on)
			std::cout<<"After we pick a value, update blocks"<<std::endl;
		
		update_blocks(*cur_spot, SET);
		cur_spot = &pick_spot_from_block();

	}
}

void SudokuBoard::output_result()
{
	for(int i = 0; i < BOARD_COLUMN; i++)
	{
		for(int j = 0; j < BOARD_LINES; j++)
		{
			std::cout << spot_array[i][j].show_val() << " ";
			if(j %3 == 2 && j != 8 )
				std::cout << "|";
 		}
		std::cout << std::endl;
		if(i %3 == 2 && i !=8)
			std::cout << "-------------------"<<std::endl;
	}
}

int SudokuBoard::spot::guess_val()
{
	int i = 1;
	
	if(last_guess != -1)
		i = history[last_guess] + 1;

	if(potential_left == 0)
	{
		//When we get here, the spot val is 0, and yet no
		//potential value we can fit in, so it is a dead end.
		//We need to tell SudokuBoard::run() to rollback.
		return -1;
	}
	
	for(; i < SPOT_BITMAP;i++)
	{
		if(bitmap[0][i] == 0  && bitmap[1][i] == 0 && bitmap[2][i] == 0)
		{
			//we pick number i
			history[++last_guess] = i;
			potential_left--;
			bitmap[0][i] = bitmap[1][i] = bitmap[2][i] =1;
			val = i;

			return val;
		}
	}

	//We have tried out all the possible values.
	//We need to rollback.
	return -1;
}

void SudokuBoard::spot::clear_history()
{
	for(int i = 0; i < TRY_NUMS;i++)
	{
		if(history[i] != 0)
		{
			bitmap[0][history[i]] = bitmap[1][history[i]] = bitmap[2][history[i]] = 0;
			potential_left++;
			history[i] = 0;
		}else
			break;
	}

	val =0;
	last_guess = -1;
}

void SudokuBoard::spot::setup(int val, int x, int y)
{
	this->val = val;
	std::memset(bitmap, 0, SPOT_BITMAP*sizeof(int)*BLOCK_TYPES);
	potential_left = TRY_NUMS;
	std::memset(history, 0, TRY_NUMS*sizeof(int));
	last_guess = -1;
	this->x = x;
	this->y = y;
}

void SudokuBoard::spot::display()
{
	std::cout << "Spot=" <<x<<","<<y<<",  val=" <<val <<",  potential_left=";
		std::cout<<potential_left<<", last_guess="<<last_guess<<std::endl;
	
	for(int j = 0;j<3;j++)
	{
		std::cout<<"bitmap"<<j<<"=";
		for(int i = 0; i < 10;i++)
		{
			std::cout <<bitmap[j][i] <<" ";
		}
		std::cout<<std::endl;	
	}
	std::cout<<std::endl<<"history=";
	for(int i = 0; i < 9;i++)
	{
		std::cout <<history[i] <<" ";
	}
	std::cout<<std::endl;
}

void SudokuBoard::spot::clear_bitmap()
{
	std::memset(bitmap, 0, SPOT_BITMAP*sizeof(int)*BLOCK_TYPES);
	potential_left = TRY_NUMS;
}

void SudokuBoard::spot::update_bitmap(int bit, bitmap_action action, block_type btype)
{
	if(action == SET)
	{
		if(bitmap[0][bit] == 0 && bitmap[1][bit] == 0 && bitmap[2][bit] == 0)
		{
			potential_left--;
		}
		bitmap[btype][bit] = 1;
	}else if(action == CLEAR)
	{
		bitmap[btype][bit] = 0;
		if(bitmap[0][bit] == 0 && bitmap[1][bit] == 0 && bitmap[2][bit] == 0)
		{
			potential_left++;
		}
	}	
}

void SudokuBoard::block_line::setup(block_type type, int index)
{
	this->btype = type;
	this->index = index;
	this->empty_vals = BLOCK_ENTRIES;	
	std::memset(bitmap, 0, BLOCK_BITMAP*sizeof(int)); 
}

void SudokuBoard::block_line::display()
{
	std::cout<< "Block_type=" << btype <<",  index=" <<index<<",  empty_vals="<<empty_vals <<std::endl;
	std::cout<<"Bitmap=";
	for(int i = 0; i < 10; i++)
	{
		std::cout<<bitmap[i]<<" ";
	}
	std::cout<<std::endl;
}	

void SudokuBoard::block_line::update_bitmap(int bit, bitmap_action action)
{
	if(action == SET)
	{
		bitmap[bit] = 1;
		empty_vals--;
	}else
	{
		bitmap[bit] = 0;
		empty_vals++;
	}
}