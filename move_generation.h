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
size_t GenerateMoves(const State&, Move* moves );

bool isCheck(const State& state, bool turn);

size_t JumperMoves(
    const State& s,
    Move* moves,
    Piece p,
    const Bitboard* attacks,
    const Bitboard& current_occupation);

template <std::size_t NumDirections>
inline std::size_t SliderMoves(
    const State& state,
    Move* moves,
    const Bitboard& sliderPieces,
    const std::array<std::pair<int, int>, NumDirections>& directions,
    const Bitboard& currentOccupancy,
    const Bitboard& otherOccupancy);