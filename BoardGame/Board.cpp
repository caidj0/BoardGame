#include "pch.h"
#include "Board.h"


using namespace std;

Board::Board(int m, int n, int color) :m(m), n(n), color(color),randint(0,color - 1),engine(time(NULL))
{
	board.resize(m, vector<int>(n,0));
}

void Board::spread()
{
	for(auto i = board.begin();i != board.end();i++)
		for(auto j = i->begin();j != i->end();j++)
			*j = randint(engine);
}

void Board::flipRow(int row)
{
	for (auto i = board[row].begin(); i != board[row].end(); i++)
		*i = (*i + 1) % color;
}

void Board::flipColumn(int column)
{
	for (auto i = board.begin(); i != board.end(); i++)
		(*i)[column] = ((*i)[column] + 1) % color;
}

void Board::flipOne(int x, int y)
{
	board[x][y] = (board[x][y] + 1) % color;
}

