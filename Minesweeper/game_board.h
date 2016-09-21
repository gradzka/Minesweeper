#include <iostream>
#include <stdio.h>
#include "time.h"
#include <string.h>

struct field
{
	int value;		//-1 mine, 0..8 mine neighbour
	bool flagged;	//flag is set
	bool discovered;//player discover the value of field
	bool last_clicked; //false - not clicked
};
#pragma once
class game_board
{
private:
	int rows;
	int columns;
	int mines_number;
	std::string beg_int_exp_cus;
	field **fields;
	void create_fields(); //create matrix
	void rand_mines(); //fills matrix with randomly placed mines
	void neighbours_mines(); //change neighbour mines fields with proper value
	bool victory = false; //variable used to change or not main smile
	bool END_OF_GAME = false;
	int no_flagged_mines_number; //used in counter of remaining mines
public:	
	game_board(std::string beg_int_exp);
	game_board(int rows, int columns, int mines_number);
	~game_board();

	int get_columns(){ return columns; }
	int get_rows(){ return rows; }
	int get_mines_number(){ return mines_number; }
	std::string get_beg_int_exp_cus(){ return beg_int_exp_cus; }
	field &get_fields(int x_pos, int y_pos){ return fields[x_pos][y_pos]; }
	void change_victory(){ victory = true; }
	bool get_victory(){ return victory; }
	void change_END_OF_GAME(){ END_OF_GAME = true; }
	bool get_END_OF_GAME(){ return END_OF_GAME; }
	void show_fields();
	int get_no_flagged_mines_number(){ return no_flagged_mines_number; }
	void add_one_to_no_flagged_mines_number(){ no_flagged_mines_number++; }
	void substract_one_from_no_flagged_mines_number(){ no_flagged_mines_number--; }
};