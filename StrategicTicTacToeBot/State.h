#pragma once
#include <map>
#include <vector>
#include <bitset>
#include "Board.h"
#include "Settings.h"

#ifdef _DEBUG
#define DEBUG(...) __VA_ARGS__
#else
#define DEBUG(...) 
#endif

extern float EVALUATIONS_ARRAY[81];

int GetBestMove(Board board);

template<int T>
float Minimax(int depth, Board board = { 0, 0, true, -1 }, bool addEvals = true
	ALPHA_BETA_ONLY(, float alpha = -INFINITY, float beta = INFINITY));

template float Minimax<WPrSum>(int depth, Board board, bool addEvals ALPHA_BETA_ONLY(, float alpha, float beta));
