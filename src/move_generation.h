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
	TMoveContainer &moves,
	const Bitboard& pawnBB,
	const Bitboard& moveOccupancy,
	const Bitboard& enemyOccupancy,
	bool whitePieces);

template <Piece _Piece>
void JumperMoves(
    const State& state,
	TMoveContainer &moves,
	const Bitboard& pieceBB,
    const Bitboard& moveOccupation);

template <Piece _Piece>
void SliderMoves(
    const State& state,
	TMoveContainer &moves,
    const Bitboard& sliderPieces,
    const Bitboard& moveOccupancy,
    const Bitboard& enemyOccupancy);

void KingCastling(
	const State& state,
	TMoveContainer &moves,
	const Bitboard& kingBB,
	const Bitboard& rookBB,
	const Bitboard& moveOccupation,
	const Bitboard& enemyOccupation,
	bool wPieces);

bool isCheck(const State& state, bool whiteKing);