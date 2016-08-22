#include <iostream>
#include <stdio.h>
#include "time.h"
#include <string.h>

#pragma once
class game_board
{
private:
	int g_b_width;
	int g_b_height;
	int **fields;
	std::string beg_int_exp;
	int mines_number;
public:
	game_board(std::string beg_int_exp);
	game_board(int X_width, int Y_height, int mines_number);
	~game_board();
	void create_fields(); //create matrix
	void show_fields();
	void rand_mines(); //fills matrix with randomly placed mines
	void neighbours_mines(); //change neighbour mines fields with proper value
};

