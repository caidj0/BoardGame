#pragma once

#include <vector>
#include <random>

class Board
{
private:
	std::default_random_engine engine;
	std::uniform_int_distribution<int> randint;
public:
	int m, n, color;
	std::vector<std::vector<int>> board;
	Board(int m, int n, int color);
	void spread();
	void flipRow(int row);
	void flipColumn(int column);
	void flipOne(int x, int y);
};

