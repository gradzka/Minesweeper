#pragma once
#include <string>
#include "windows.h"
struct score
{
	std::string name;
	int time;
};

class HighScore
{
private:
	bool Started_TIMER = false;
	unsigned int TIMER = 0;
	score HighScores[9];
	TCHAR PlayerName[17];
public:
	HighScore();
	~HighScore();
	
	void reset_HighScores();
	void load_HighScores();
	bool get_Started_TIMER(){ return Started_TIMER; }
	void change_Started_TIMER(){ Started_TIMER = true; }
	unsigned int get_TIMER(){ return TIMER; }
	void add_one_to_TIMER(){ TIMER++; }
	LPCTSTR get_i_HighScores_name(int i){ return HighScores[i].name.c_str(); }
	int get_i_HighScores_time(int i){ return HighScores[i].time; }
	TCHAR* get_PlayerName(){ return PlayerName; }
	void change_i_HighScores_name(int index_first, int index_second){ HighScores[index_first].name = get_i_HighScores_name(index_second); }
	void change_i_HighScores_name(int index_first, LPCSTR new_name){ HighScores[index_first].name = new_name; }
	void change_i_HighScores_time(int index_first, int index_second){ HighScores[index_first].time = get_i_HighScores_time(index_second); }
	void change_i_HighScores_time_2(int index_first, int new_time){ HighScores[index_first].time = new_time; }
};

