#pragma once
#include <bitset>

struct Board
{
	std::bitset<81> x;
	std::bitset<81> o;
	bool m_Turn;
	int m_NextMove;
	std::bitset<9> bx;
	std::bitset<9> bo;

	int LoadEval();
	bool IsAvailable(int move);
	Board MakeMove(int move);
	std::string ToString() const;

	bool operator==(const Board& other) const;
	bool operator<(const Board& other) const;
};

std::ostream& operator <<(std::ostream& output, const Board& data);
