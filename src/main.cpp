#include <iostream>
#include <iomanip>

#include "string_transforms.h"
#include "move_generation.h"

int main()
{
    std::string fen = "k7/8/6K1/8/8/8/2PPpN2/2BQr3 w - - 0 1";
    State state = StateFromFen(fen);
    Move moves[64];
    std::size_t numMoves = GenerateMoves(state, moves);

    for(int i=0; i<numMoves; i++)
    {
        auto move = moves[i];
        std::cout << AsUci(move) << std::endl;
    }
    return 0;
}