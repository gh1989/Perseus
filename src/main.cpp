#include <iostream>
#include <iomanip>

#include "string_transforms.h"
#include "move_generation.h"

int main()
{
    std::string fen = "8/8/8/8/8/8/K2pR2P/R7 w - - 0 1";
    State state = StateFromFen(fen);
    auto moves = GenerateMoves(state);

    for(auto m : moves)
    {
        std::cout << AsUci(m) << std::endl;
    }
    return 0;
}