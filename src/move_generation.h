#pragma once

#include <list>

#include "bitboard.h"
#include "geometry.h"
#include "move.h"
#include "state.h"
#include "types.h"

typedef std::list<Move> TMoveContainer;

/* Move generation */
TMoveContainer GenerateMoves(const State&);

void PawnMoves(
	const State& state,
	TMoveContainer &moves);

template <Piece _Piece>
void JumperMoves(
    const State& state,
	TMoveContainer &moves);

template <Piece _Piece>
void SliderMoves(
    const State& state,
	TMoveContainer &moves);

void KingCastling(
	const State& state,
	TMoveContainer &moves);

bool isCheck(const State& state, bool whiteKing);

bool checkLegal(const State& state, Move move);