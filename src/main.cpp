#include <iostream>
#include <iomanip>

#include "string_transforms.h"
#include "move_generation.h"

int main()
{
    std::string fen = "k7/4P3/8/1p6/2P5/8/8/7K w - - 0 1";
    State state = StateFromFen(fen);
    Move moves[64];
    GenerateMoves(state, moves);

    for(int i=0; i<64; i++)
    {
        auto move = moves[i];
        if(move == 0)
            break;
        std::cout << AsUci(move) << std::endl;
    }
    return 0;
}