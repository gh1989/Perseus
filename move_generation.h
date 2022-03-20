#pragma once
#include "bitboard.h"
#include "geometry.h"
#include "move.h"
#include "state.h"
#include "types.h"

template<class InputIt, class T, class BinaryOperation>
constexpr
T accumulate(InputIt first, InputIt last, T init,
	BinaryOperation op)
{
	for (; first != last; ++first) {
		init = op(std::move(init), *first); 
	}
	return init;
}

/* Move generation */
size_t GenerateMoves(const State&, Move*);

bool isCheck(const State& state, bool turn);

size_t JumperMoves(
	const State& s,
	Move* moves,
	Piece p,
	const Bitboard* attacks,
	Bitboard current_occupation);

size_t SliderMoves(
	const State& s,
	Move* moves,
	Bitboard piecebb,
	const std::pair<int, int>* directions,
	Bitboard current_occupation,
	Bitboard other_occupation);