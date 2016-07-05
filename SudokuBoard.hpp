#ifndef  SUDOKUBOARD_HPP
#define SUDOKUBOARD_HPP

#include <vector>       // std::vector
#include <stack>

#define BOARD_LINES	9
#define BOARD_COLUMN	9
#define BLOCK_NUM		9
#define BLOCK_ENTRIES	9
#define BLOCK_BITMAP	10

#define TRY_NUMS		9
#define SPOT_BITMAP		10

enum block_type
{
	BLOCK,
	VER_LINE,
	HOR_LINE,
	BLOCK_TYPES
};

enum bitmap_action
{
	SET,
	CLEAR
};

class SudokuBoard
{
public:
	SudokuBoard(int array[][BOARD_COLUMN]);
	int run();
	void output_result();
	~SudokuBoard() {};
private:
	class spot
	{
	public:
		int show_val() {return val;};
		void clear_val() {val = 0;};
		int guess_val();
		void clear_history();
		void setup(int val,  int x, int y);
		void display();
		void clear_bitmap();
		void update_bitmap(int bit, bitmap_action action, block_type btype);

		int x,y;
	private:
		int val;
		int bitmap[BLOCK_TYPES][SPOT_BITMAP];  // 1 means number can't be selected, 0 means potential number
		int potential_left;
		int history[TRY_NUMS];
		int last_guess; 
	};
	class block_line
	{
	public:
		block_type type() {return btype;};
		void setup(block_type type, int index);
		void display();
		void update_bitmap(int bit, bitmap_action action);
		int get_index() { return index; };	
		int get_empty_vals() const { return empty_vals;};
		
	private:
		block_type btype;
		int index;
		int empty_vals;
		int bitmap[BLOCK_BITMAP];
	};

	struct compare
	{
      		bool operator()(const block_line* a, const block_line* b) {
        			return a->get_empty_vals() < b->get_empty_vals();
      		}	
	};

	void prepare();
	spot &pick_spot_from_block();
	void update_blocks(spot& spot, bitmap_action action);	
	
	spot spot_array[BOARD_LINES][BOARD_COLUMN];
	block_line bl_array[BLOCK_TYPES][BLOCK_NUM];
	std::vector<block_line*> block_vec;
	std::stack<spot*> stack;
	int blanks;
};
#endif /*SUDOKUBOARD_HPP*/