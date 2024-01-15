#include "Board.h"
#include "Timer.h"
#include <bitset>
#include <iostream>
#include <fstream>
#include <random>

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

unsigned char wins9T[9] = { 146,136,133,80,75,68,49,40,38 };


std::map<SmallBoard, std::pair<float, float>> SmallBoard::outcomes = {};

template<int T>
float Board::LoadEval()
{
	return 0.f;
}



template<>
float Board::LoadEval<EvalType::WPrSum>()
{
	//Timer t2("");

	float probs[16] = { 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f,
						1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f };

	unsigned char xro = 0, oro = 0;
	for (int i = 0; i < 9; i++)
	{
		if (bx[i]) oro |= wins9T[i];
		if (bo[i]) xro |= wins9T[i];
	}

	for (int i = 0; i < 8; i++)
	{
		if ((1 << i) & xro) probs[i] = 0.f;
		if ((1 << i) & oro) probs[i + 8] = 0.f;

	}

	std::pair<float, float> winrate;
	for (int i = 0; i < 9; i++)
	{
		if (!(bx | bo)[i])
		{
			winrate = SmallBoard::outcomes[{((x >> (9 * i))& std::bitset<81>{511}).to_ulong(),
				((o >> (9 * i))& std::bitset<81>{511}).to_ulong()}];

			for (int j = 0; j < 8; j++)
			{
				if ((1 << j) & ~xro & wins9T[i]) probs[j] *= winrate.first;
				if ((1 << j) & ~oro & wins9T[i]) probs[j + 8] *= winrate.second;

			}
		}
	}

	float ret = 0;
	for (int j = 0; j < 8; j++) ret += probs[j] - probs[j + 8];
	//for (int i = 0; i < 16; i++) std::cout << probs[i] << ' ';
	//std::cout << '\n';
	return ret / 9.f;

	//return (evals[6] * evals[7] * evals[8] - evals[6 + 9] * evals[7 + 9] * evals[8 + 9] +
	//		evals[3] * evals[4] * evals[5] - evals[3 + 9] * evals[4 + 9] * evals[5 + 9] +
	//		evals[0] * evals[1] * evals[2] - evals[0 + 9] * evals[1 + 9] * evals[2 + 9] +
	//		evals[2] * evals[5] * evals[8] - evals[2 + 9] * evals[5 + 9] * evals[8 + 9] +
	//		evals[1] * evals[4] * evals[7] - evals[1 + 9] * evals[4 + 9] * evals[7 + 9] +
	//		evals[0] * evals[3] * evals[6] - evals[0 + 9] * evals[3 + 9] * evals[6 + 9] +
	//		evals[0] * evals[4] * evals[8] - evals[0 + 9] * evals[4 + 9] * evals[8 + 9] +
	//		evals[2] * evals[4] * evals[6] - evals[2 + 9] * evals[4 + 9] * evals[6 + 9]) / 9.f;
}

// Random Trials approach
template<>
float Board::LoadEval<EvalType::RandomTrials>()
{
	//Timer t2("");

	static std::default_random_engine gen;
	static std::uniform_real_distribution<double> distribution(0., 1.);

	std::pair<float, float> winrate;
	float ret = 0;

	for (int c = 0; c < 100; c++)
	{
		SmallBoard sb = { bx, bo };
		for (int i = 0; i < 9; i++)
		{
			if (!(bx | bo)[i])
				winrate = SmallBoard::outcomes[{((x >> (9 * i))& std::bitset<81>{511}).to_ulong(),
					((o >> (9 * i))& std::bitset<81>{511}).to_ulong()}];
				double num = distribution(gen);
				if (num < winrate.first)
					sb.x.set(i);
				else if (num < winrate.first + winrate.second)
					sb.o.set(i);
		}
		ret += (sb.GetXState() ? 0.01f : 0.0f) - (sb.GetOState() ? 0.01f : 0.0f);
		if (sb.GetXState() && sb.GetOState())
			c--;
	}

	return ret;
}

// Linear Model approach
const float SQUARE_VALUES[9] = { 0.216, 0.137, 0.216, 0.137, 0.287, 0.137, 0.216, 0.137, 0.216 };

template<>
float Board::LoadEval<EvalType::PrLM>()
{
	Timer t2("");

	float ret = 0;

	std::pair<float, float> winrate;
	for (int i = 0; i < 9; i++)
	{
		switch (bx[i] + 2 * bo[i])
		{
		case 1:
			ret += SQUARE_VALUES[i];
			break;
		case 2:
			ret -= SQUARE_VALUES[i];
			break;
		case 0:
			winrate = SmallBoard::outcomes[{((x >> (9 * i))& std::bitset<81>{511}).to_ulong(),
				((o >> (9 * i))& std::bitset<81>{511}).to_ulong()}];

			ret += SQUARE_VALUES[i] * (winrate.first - winrate.second);
			break;
		}


		//if (bx[i]) ret += SQUARE_VALUES[i];
		//else if (bo[i]) ret -= SQUARE_VALUES[i];
		//else
		//{
		//	winrate = SmallBoard::outcomes[{((x >> (9 * i))& std::bitset<81>{511}).to_ulong(),
		//		((o >> (9 * i))& std::bitset<81>{511}).to_ulong()}];
		//
		//	ret += SQUARE_VALUES[i] * (winrate.first - winrate.second);
		//}
	}

	return ret;

	//return (evals[6] * evals[7] * evals[8] - evals[6 + 9] * evals[7 + 9] * evals[8 + 9] +
	//		evals[3] * evals[4] * evals[5] - evals[3 + 9] * evals[4 + 9] * evals[5 + 9] +
	//		evals[0] * evals[1] * evals[2] - evals[0 + 9] * evals[1 + 9] * evals[2 + 9] +
	//		evals[2] * evals[5] * evals[8] - evals[2 + 9] * evals[5 + 9] * evals[8 + 9] +
	//		evals[1] * evals[4] * evals[7] - evals[1 + 9] * evals[4 + 9] * evals[7 + 9] +
	//		evals[0] * evals[3] * evals[6] - evals[0 + 9] * evals[3 + 9] * evals[6 + 9] +
	//		evals[0] * evals[4] * evals[8] - evals[0 + 9] * evals[4 + 9] * evals[8 + 9] +
	//		evals[2] * evals[4] * evals[6] - evals[2 + 9] * evals[4 + 9] * evals[6 + 9]) / 9.f;
}


//  Specificity of win approach
template<>
float Board::LoadEval<EvalType::SufficientPrEval>()
{
	//Timer t2("eval2");

	std::bitset<9> bxPred = bx;
	std::bitset<9> boPred = bo;

	std::pair<float, float> winrate;
	for (int i = 0; i < 9; i++)
	{
		if (!(bx | bo)[i])
		{
			winrate = SmallBoard::outcomes[{((x >> (9 * i))& std::bitset<81>{511}).to_ulong(),
				((o >> (9 * i))& std::bitset<81>{511}).to_ulong()}];

			if (winrate.first >= 0.5) // this parameter can be tuned
				bxPred.set(i);
			else if (winrate.second >= 0.5)
				boPred.set(i);
		}
	}
	winrate = SmallBoard::outcomes[{ bxPred, boPred }];
	//for (int i = 0; i < 16; i++) std::cout << probs[i] << ' ';
	//std::cout << '\n';
	return winrate.first - winrate.second;
}


int Board::GetWinState()
{
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
	return b;  // todo: make m_NextMove = -1 when lead to a full but drawn board
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

bool SmallBoard::GetXState()
{
	return (((~x & wins9[0]) == 0) || ((~x & wins9[1]) == 0) || ((~x & wins9[2]) == 0)
			|| ((~x & wins9[3]) == 0) || ((~x & wins9[4]) == 0) || ((~x & wins9[5]) == 0)
			|| ((~x & wins9[6]) == 0) || ((~x & wins9[7]) == 0));
}

bool SmallBoard::GetOState()
{
	return (((~o & wins9[0]) == 0) || ((~o & wins9[1]) == 0) || ((~o & wins9[2]) == 0)
		|| ((~o & wins9[3]) == 0) || ((~o & wins9[4]) == 0) || ((~o & wins9[5]) == 0)
		|| ((~o & wins9[6]) == 0) || ((~o & wins9[7]) == 0));
}

bool SmallBoard::operator<(const SmallBoard& other) const
{
	return x.to_ulong() == other.x.to_ulong() ?
		   o.to_ulong() < other.o.to_ulong() :
		   x.to_ulong() < other.x.to_ulong();
}

void SmallBoard::LoadOutcomes()
{
	Timer t("load outcomes");

	std::ifstream file("Assets\\PTTTstates.dat", std::ios::binary);

	char data[12];
	while (file.good())
	{
		file.read((char*)&data, 12);
		outcomes.insert({ { std::bitset<9>(*(short*)&data[0]), std::bitset<9>(*(short*)&data[2]) },
						  { *(float*)&data[4], *(float*)&data[8] } });
	}

	file.close();
}
