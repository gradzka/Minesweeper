#include "HighScore.h"
#include <fstream>

HighScore::HighScore()
{
}


HighScore::~HighScore()
{
}

void HighScore::reset_HighScores()
{
	std::fstream file;
	file.open("HighScores.txt", std::ios::out | std::ios::trunc);
	for (int i = 0; i < 9; i++)
	{
		if (i == 0)
		{
			file << "Beginner\n";
		}
		if (i == 3)
		{
			file << "\nIntermediate\n";
		}
		if (i == 6)
		{
			file << "\nExpert\n";
		}
		file << "Anonymous 999\n";
		HighScores[i].name = "Anonymous";
		HighScores[i].time = 999;
	}
	file.close();
}
void HighScore::load_HighScores()
{
	std::fstream file;
	file.open("HighScores.txt", std::ios::in);
	if (!file.is_open())
	{
		reset_HighScores();
	}
	else
	{
		for (int i = 0; i < 9; i++)
		{
			if (i % 3 == 0)
			{
				file >> HighScores[i].name;
			}
			file >> HighScores[i].name;
			file >> HighScores[i].time;
		}
		file.close();
	}
}