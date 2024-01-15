#include "Board.h"
#include <bitset>
#include <iostream>

std::bitset<81> wins[8] = { std::bitset<81>{"111000000"},
						    std::bitset<81>{"000111000"},
						    std::bitset<81>{"000000111"},
						    std::bitset<81>{"100100100"},
						    std::bitset<81>{"010010010"},
						    std::bitset<81>{"001001001"},
						    std::bitset<81>{"100010001"},
						    std::bitset<81>{"001010100"} };

std::bitset<9> wins9[8] = { std::bitset<9>{"111000000"},
							std::bitset<9>{"000111000"},
							std::bitset<9>{"000000111"},
							std::bitset<9>{"100100100"},
							std::bitset<9>{"010010010"},
							std::bitset<9>{"001001001"},
							std::bitset<9>{"100010001"},
							std::bitset<9>{"001010100"} };



int Board::LoadEval()
{
	std::bitset<9> bx; // todo: store these values as members
	std::bitset<9> bo;
	for (int i = 0; i < 9; i++)
	{
		switch ((((~o & wins[0] << (9 * i)) == 0) || ((~o & wins[1] << (9 * i)) == 0) || ((~o & wins[2] << (9 * i)) == 0)
			  || ((~o & wins[3] << (9 * i)) == 0) || ((~o & wins[4] << (9 * i)) == 0) || ((~o & wins[5] << (9 * i)) == 0)
			  || ((~o & wins[6] << (9 * i)) == 0) || ((~o & wins[7] << (9 * i)) == 0)) ? -1 :
			    (((~x & wins[0] << (9 * i)) == 0) || ((~x & wins[1] << (9 * i)) == 0) || ((~x & wins[2] << (9 * i)) == 0)
			  || ((~x & wins[3] << (9 * i)) == 0) || ((~x & wins[4] << (9 * i)) == 0) || ((~x & wins[5] << (9 * i)) == 0)
			  || ((~x & wins[6] << (9 * i)) == 0) || ((~x & wins[7] << (9 * i)) == 0)) ? 1 : 0)
		{
		case 1:
			bx[i] = 1;
			break;
		case -1:
			bo[i] = 1;
			break;
		default:
			break;
		}
	}
	return (((~bo & wins9[0]) == 0) || ((~bo & wins9[1]) == 0) || ((~bo & wins9[2]) == 0)
		|| ((~bo & wins9[3]) == 0) || ((~bo & wins9[4]) == 0) || ((~bo & wins9[5]) == 0)
		|| ((~bo & wins9[6]) == 0) || ((~bo & wins9[7]) == 0)) ? -1 :
		(((~bx & wins9[0]) == 0) || ((~bx & wins9[1]) == 0) || ((~bx & wins9[2]) == 0)
			|| ((~bx & wins9[3]) == 0) || ((~bx & wins9[4]) == 0) || ((~bx & wins9[5]) == 0)
			|| ((~bx & wins9[6]) == 0) || ((~bx & wins9[7]) == 0)) ? 1 : 0;
}

bool Board::IsAvailable(int move)
{
	if (m_NextMove == -1)	return !(x | o)[move] && !(bx | bo)[move / 9];
	else return !(x | o)[9 * m_NextMove + move];
}

Board Board::MakeMove(int move)
{
	std::bitset<81> m; m[(m_NextMove == -1) ? move : 9 * m_NextMove + move] = 1;
	Board b = { m_Turn ? x | m : x, m_Turn ? o : o | m, !m_Turn, -1, bx, bo };
	int i = 9 * ((m_NextMove == -1) ? move / 9 : m_NextMove);
	if (((~b.o & wins[0] << i) == 0) || ((~b.o & wins[1] << i) == 0) || ((~b.o & wins[2] << i) == 0)
		|| ((~b.o & wins[3] << i) == 0) || ((~b.o & wins[4] << i) == 0) || ((~b.o & wins[5] << i) == 0)
		|| ((~b.o & wins[6] << i) == 0) || ((~b.o & wins[7] << i) == 0)) b.bo[i / 9] = 1;
	if (((~b.x & wins[0] << i) == 0) || ((~b.x & wins[1] << i) == 0) || ((~b.x & wins[2] << i) == 0)
		|| ((~b.x & wins[3] << i) == 0) || ((~b.x & wins[4] << i) == 0) || ((~b.x & wins[5] << i) == 0)
		|| ((~b.x & wins[6] << i) == 0) || ((~b.x & wins[7] << i) == 0)) b.bx[i / 9] = 1;
	b.m_NextMove = ((b.bo | b.bx)[(m_NextMove == -1) ? move % 9 : move] ? -1 : ((m_NextMove == -1) ? move % 9 : move));
	return b;
}

std::string Board::ToString() const
{
	std::string ret = "";
	for (int i = 0; i < 81; i += 18)
	{
		for (; i % 27 < 9; i += 3)
		{
			ret += (x[i + 0] ? 'X' : o[i + 0] ? 'O' : ' ');
			ret += (x[i + 1] ? 'X' : o[i + 1] ? 'O' : ' ');
			ret += (x[i + 2] ? 'X' : o[i + 2] ? 'O' : ' ');
			ret += (x[i + 9] ? 'X' : o[i + 9] ? 'O' : ' ');
			ret += (x[i + 10] ? 'X' : o[i + 10] ? 'O' : ' ');
			ret += (x[i + 11] ? 'X' : o[i + 11] ? 'O' : ' ');
			ret += (x[i + 18] ? 'X' : o[i + 18] ? 'O' : ' ');
			ret += (x[i + 19] ? 'X' : o[i + 19] ? 'O' : ' ');
			ret += (x[i + 20] ? 'X' : o[i + 20] ? 'O' : ' ');
			ret += (i == 60 ? "" : "\n");
		}
	}
	return ret;
}

bool Board::operator==(const Board& other) const
{
	return x == other.x && o == other.o && m_NextMove;
}

bool Board::operator<(const Board& other) const
{
	return x.to_string() == other.x.to_string() ? o.to_string() == other.o.to_string() ?
		m_NextMove < other.m_NextMove : o.to_string() < other.o.to_string() : x.to_string() < other.x.to_string();
}

std::ostream& operator <<(std::ostream& output, const Board& data)
{
	return output << data.ToString();
}
