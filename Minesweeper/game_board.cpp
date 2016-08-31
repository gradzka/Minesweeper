#include "game_board.h"

game_board::game_board(std::string beg_int_exp_cus)
{
	fields = NULL;
	this->beg_int_exp_cus = beg_int_exp_cus;

	if (this->beg_int_exp_cus.compare("Beginner") == 0)
	{
		g_b_columns = 9;
		g_b_rows = 9;
		mines_number = 10;
	}
	if (this->beg_int_exp_cus.compare("Intermediate") == 0)
	{
		g_b_columns = 16;
		g_b_rows = 16;
		mines_number = 40;
	}
	if (this->beg_int_exp_cus.compare("Expert") == 0)
	{
		g_b_columns = 30;
		g_b_rows = 16;
		mines_number = 99;
	}

	this->create_fields();

	srand(time(NULL));
	this->rand_mines();
	this->neighbours_mines();
}
game_board::game_board(int rows, int columns, int mines_number)
{
	/*Player should type board size between 9x9 and 30x24
	and mines number between 10 and 667.
	Max mines number is related with board size, so if board size is AxB,
	max number is (A-1)x(B-1)
	*/
	fields = NULL;
	beg_int_exp_cus = "Custom";

	if (columns < 9) columns = 9;
	else if (columns > 30) columns = 30;

	if (rows < 9) rows = 9;
	else if (rows > 24) rows = 24;

	g_b_columns = columns;
	g_b_rows = rows;

	this->create_fields();

	int max_mines_number = 0;
	max_mines_number = (g_b_columns - 1)*(g_b_rows - 1);

	if (mines_number < 10) mines_number = 10;
	else if (mines_number>max_mines_number) mines_number = max_mines_number;

	this->mines_number = mines_number;

	srand(time(NULL));
	this->rand_mines();
	this->neighbours_mines();
}
game_board::~game_board()
{
	for (int i = 0; i < g_b_rows; i++)
	{
		delete[]fields[i];
	}
	delete[]fields;
}
void game_board::create_fields()
{
	fields = new field*[g_b_rows];
	for (int i = 0; i < g_b_rows; i++)
	{
		fields[i] = new field[g_b_columns];
		for (int j = 0; j < g_b_columns; j++)
		{
			fields[i][j].value = 0;
			fields[i][j].flagged = false;
			fields[i][j].discovered = false;
		}
	}
}
void game_board::show_fields()
{
	for (int i = 0; i < g_b_rows; i++)
	{
		for (int j = 0; j < g_b_columns; j++)
		{
			std::cout << fields[i][j].value << "\t";
		}
		std::cout << std::endl;
	}
}
void game_board::rand_mines()
{
	int X = 0;
	int Y = 0;

	char bufor[200];

	for (int i = 0; i < mines_number; i++)
	{
		do
		{
			X = rand() % g_b_rows;
			Y = rand() % g_b_columns;
			/*sprintf_s(bufor, "(%i, %i)", X, Y);
			std::cout << bufor << std::endl;*/
		} while (fields[X][Y].value == -1);

		fields[X][Y].value = -1;
	}
}
void game_board::neighbours_mines()
{
	for (int i = 0; i < g_b_rows; i++)
	{
		for (int j = 0; j < g_b_columns; j++)
		{
			if (fields[i][j].value == -1)
			{
				if ((i - 1 >= 0) && (fields[i - 1][j].value != -1))
				{
					fields[i - 1][j].value++; //up
				}
				if ((j + 1 < g_b_columns) && (fields[i][j + 1].value != -1))
				{
					fields[i][j + 1].value++; //right
				}
				if ((i + 1 < g_b_rows) && (fields[i + 1][j].value != -1))
				{
					fields[i + 1][j].value++; //down
				}
				if ((j - 1 >= 0) && (fields[i][j - 1].value != -1))
				{
					fields[i][j - 1].value++; //left
				}
				if ((i - 1 >= 0) && (j - 1 >= 0) && (fields[i - 1][j - 1].value != -1)) //upper left diagonally
				{
					fields[i - 1][j - 1].value++;
				}
				if ((i - 1 >= 0) && (j + 1 < g_b_columns) && (fields[i - 1][j + 1].value != -1)) //upper right diagonally
				{
					fields[i - 1][j + 1].value++;
				}
				if ((i + 1 < g_b_rows) && (j - 1 >= 0) && (fields[i + 1][j - 1].value != -1)) //bottom left diagonally
				{
					fields[i + 1][j - 1].value++;
				}
				if ((i + 1 < g_b_rows) && (j + 1 < g_b_columns) && (fields[i + 1][j + 1].value != -1)) //bottom right diagonally
				{
					fields[i + 1][j + 1].value++;
				}
			}
		}
	}
}