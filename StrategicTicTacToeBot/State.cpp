#include "State.h"
#include <iostream>
#include "Timer.h"

float EVALUATIONS_ARRAY[81] = {};

int GetBestMove(Board board)
{
	if (board.GetWinState() != 0)
		return -1;

	float val = board.m_Turn ? -INFINITY : INFINITY;
	int move = -1;

	for (int i = 0; i < ((board.m_NextMove == -1) ? 81 : 9); i++)
		if (board.IsAvailable(i))
		{
			if (board.m_Turn ? EVALUATIONS_ARRAY[i] > val
				: EVALUATIONS_ARRAY[i] < val)
			{
				val = EVALUATIONS_ARRAY[i];
				move = i;
			}
		}
	
	return move;
}

template<int T>
float Minimax(int depth, Board board, bool addEvals ALPHA_BETA_ONLY(, float alpha, float beta))
{
	float val = board.m_Turn ? -INFINITY : INFINITY;

	if (board.GetWinState() != 0)
	{
#ifdef BULLYING
		val = (1.f / (depth + 1) + 1) * board.GetWinState();
#else
		val = (depth + 1) * board.GetWinState();
#endif
		return val;
	}
	if (depth == 0)
	{
		val = board.LoadEval<T>();
		return val;
	}
	for (int i = 0; i < ((board.m_NextMove == -1) ? 81 : 9); i++)
		if (board.IsAvailable(i))
		{
			float mm = Minimax<WPrSum>(depth - 1, board.MakeMove(i), false ALPHA_BETA_ONLY(, alpha, beta));
			if (addEvals) EVALUATIONS_ARRAY[i] = mm;

			if (board.m_Turn)
			{
				val = std::max(mm, val);
				ALPHA_BETA_ONLY(alpha = std::max(mm, alpha);)
			}
			else
			{
				val = std::min(mm, val);
				ALPHA_BETA_ONLY(beta = std::min(mm, beta);)
			}

			ALPHA_BETA_ONLY(
				if (beta <= alpha)
					break;
			)
		}
	if (val == INFINITY || val == -INFINITY)
	{
		if (board.m_NextMove == -1) // this isn't good enough when you are lead to a board that is full
			val = 0;
		else
		{
			board.m_NextMove = -1;
			return Minimax<WPrSum>(depth, board, addEvals ALPHA_BETA_ONLY(, alpha, beta));
		}
	}

	return val;
}



