#pragma once

#include "bitboard.h"
#include "geometry.h"
#include "move.h"
#include "state.h"
#include "types.h"

/* Move generation */
size_t GenerateMoves(const State&, Move* moves );

size_t PawnMoves(
	const State& state, 
	Move* moves, 
	const Bitboard& pawnBB, 
	const Bitboard& moveOccupancy, 
	const Bitboard& enemyOccupancy, 
	bool whitePieces);

template <Piece _Piece>
size_t JumperMoves(
    const State& s,
    Move* moves,
	const Bitboard& pieceBB,
    const Bitboard& moveOccupation );

template <Piece _Piece>
inline std::size_t SliderMoves(
    const State& state,
    Move* moves,
    const Bitboard& sliderPieces,
    const Bitboard& moveOccupancy,
    const Bitboard& enemyOccupancy);

size_t KingCastling(
	const State& state,
	Move *moves,
	const Bitboard& kingBB,
	const Bitboard& rookBB,
	const Bitboard& moveOccupation,
	const Bitboard& enemyOccupation,
	bool wPieces);


bool isCheck(const State& state, bool whiteKing);