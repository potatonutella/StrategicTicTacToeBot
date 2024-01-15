#include "State.h"
#include <iostream>

std::map<Board, State> State::boards = {};

State::State()
	: m_Board({ 0, 0, true }), m_Value(0), m_VChange(false)
{	
	LoadChildren();
}

State::State(Board board, State* parent)
	: m_Board(board), m_VChange(false)
{
	m_Value = m_Board.LoadEval();
	LoadChildren();

	ReinState(parent);
}

void State::ReinState(State* parent)
{
	if (!parent->m_VChange)
	{
		parent->m_Value = m_Value;
		parent->m_VChange = true;
	}
	else if (m_Board.m_Turn)
		parent->m_Value = std::min(m_Value, parent->m_Value);
	else parent->m_Value = std::max(m_Value, parent->m_Value);
}

void State::LoadChildren()
{
	if (m_Value != 0) return;
	for (int i = 0; i < ((m_Board.m_NextMove == -1) ? 81 : 9); i++)
		if (m_Board.IsAvailable(i))
		{
			Board board = m_Board.MakeMove(i);

			if (boards.count(board))
				boards.at(board).ReinState(this);
			else
				boards.insert({ board, State(board, this) });

			if (m_Value == (m_Board.m_Turn ? 1 : -1)) break;
		}
}
