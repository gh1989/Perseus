#include <iostream>
#include <iomanip>
#include <numeric>

#include "string_transforms.h"
#include "move_generation.h"

int main()
{
    std::string fen = "8/8/8/8/8/8/K2pR2P/R7 w - - 0 1";
    State state = StateFromFen(fen);
    std::list<Move> moves;

    int pieceStart = 0;
    int enemyPieceStart = 6;
    auto bit_or = [&](const Bitboard &a, const Bitboard &b) { return a | b; };
	Bitboard moveOccupation = std::accumulate<>(state.bbs + pieceStart, state.bbs + pieceStart + NUMBER_PIECES, Bitboard(0), bit_or);
	Bitboard enemyOccupation = std::accumulate<>(state.bbs + enemyPieceStart, state.bbs + enemyPieceStart + NUMBER_PIECES, Bitboard(0), bit_or);

    PawnMoves(state, moves, state.bbs[PAWN], moveOccupation, enemyOccupation, true);
    JumperMoves<KING>(state, moves, state.bbs[KING], moveOccupation);
    JumperMoves<KNIGHT>(state, moves, state.bbs[KNIGHT], moveOccupation);
    SliderMoves<ROOK>(state, moves, state.bbs[ROOK], moveOccupation, enemyOccupation);
    SliderMoves<BISHOP>(state, moves, state.bbs[BISHOP], moveOccupation, enemyOccupation);

    for(auto m : moves)
    {
        std::cout << AsUci(m) << std::endl;
    }
    return 0;
}