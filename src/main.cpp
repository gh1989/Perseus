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

    PawnMoves(state, moves);
    JumperMoves<KING>(state, moves);
    JumperMoves<KNIGHT>(state, moves);
    SliderMoves<ROOK>(state, moves);
    SliderMoves<BISHOP>(state, moves);

    for(auto m : moves)
    {
        std::cout << AsUci(m) << std::endl;
    }
    return 0;
}