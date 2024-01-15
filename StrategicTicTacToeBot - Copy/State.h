#pragma once
#include <map>
#include <vector>
#include <bitset>
#include "Board.h"

#ifdef _DEBUG
#define DEBUG(...) __VA_ARGS__
#else
#define DEBUG(...) 
#endif

class State
{
public:
	State();
	State(Board board, State* parent);
	void ReinState(State* parent);
	
	void LoadChildren();

	bool m_VChange;
	int m_Value;

	Board m_Board;

	static std::map<Board, State> boards;
private:
};
