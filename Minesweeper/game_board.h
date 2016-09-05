#include <iostream>
#include <stdio.h>
#include "time.h"
#include <string.h>

struct field
{
	int value;		//-1 mine, 0..8 mine neighbour
	bool flagged;	//flag is set
	bool discovered;//player discover the value of field
};
#pragma once
class game_board
{
private:
	int rows;
	int columns;
	int mines_number;
	std::string beg_int_exp_cus;
public:
	field **fields; //to improve, prefferd private
	
	game_board(std::string beg_int_exp);
	game_board(int rows, int columns, int mines_number);
	~game_board();

	int get_columns(){ return columns; }
	int get_rows(){ return rows; }
	int get_mines_number(){ return mines_number; }
	std::string get_beg_int_exp_cus(){ return beg_int_exp_cus; }
	void create_fields(); //create matrix
	void show_fields();
	void rand_mines(); //fills matrix with randomly placed mines
	void neighbours_mines(); //change neighbour mines fields with proper value
};

/*
int main()
{
//game_board *g_b = new game_board("Beginner");
game_board *g_b = new game_board(10,10,82);
g_b->show_fields();
delete g_b;
system("PAUSE");
return 0;
}
*/