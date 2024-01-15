#pragma once
#include <map>
#include <bitset>

struct SmallBoard
{
	std::bitset<9> x;
	std::bitset<9> o;

	bool GetXState();
	bool GetOState();

	bool operator<(const SmallBoard& other) const;

	static std::map<SmallBoard, std::pair<float, float>> outcomes;

	static void LoadOutcomes();

};

struct Board
{
	std::bitset<81> x;
	std::bitset<81> o;
	bool m_Turn;
	int m_NextMove;
	std::bitset<9> bx;
	std::bitset<9> bo;

	template<int T>
	float LoadEval();
	
	int GetWinState();
	bool IsAvailable(int move);
	Board MakeMove(int move);
	std::string ToString() const;

	bool operator==(const Board& other) const;
	bool operator<(const Board& other) const;
};

std::ostream& operator <<(std::ostream& output, const Board& data);


enum EvalType
{
	WPrSum,
	RandomTrials,
	PrLM,
	SufficientPrEval,  // stupid
};

