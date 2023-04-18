#include <iostream>
#include <iomanip>
#include <numeric>

#include "string_transforms.h"
#include "move_generation.h"

int main()
{
    State state = StateFromFen();

    // apply some moves
    state.Apply(CreateMove(e2, e4)); // white pawn moves 2 squares
    state.Apply(CreateMove(e7, e5)); // black pawn moves 2 squares
    state.Apply(CreateMove(g1, f3)); // white knight moves
    state.Apply(CreateMove(d7, d6)); // black pawn moves
    state.Apply(CreateMove(f3, e5)); // white knight captures black pawn
    state.Apply(CreateMove(d6, e5)); // black pawn captures white knight
    state.Apply(CreateMove(d2, d4)); // white pawn moves 2 squares
    state.Apply(CreateMove(e5, d4)); // black pawn captures white pawn
    state.Apply(CreateMove(g2, g3)); // white pawn moves
    state.Apply(CreateMove(d8, h4)); 
    state.Apply(CreateMove(f1, c4)); 
    state.Apply(CreateMove(h4, g4));
    state.Apply(CreateCastle(e1, g1)); 
    state.Apply(CreateMove(c8, f5)); 
    state.Apply(CreateMove(e4, e5));
    state.Apply(CreateMove(b8, c6));
    state.Apply(CreateMove(e5, e6));
    state.Apply(CreateCastle(e8, c8)); 
    state.Apply(CreateMove(e6, f7));
    state.Apply(CreateMove(g7, g5));
    state.Apply(CreatePromotion(f7, g8, ROOK));

    //PrettyPrint(state);

    state = StateFromFen("1nb1kbnr/pppppppp/8/8/8/2r1NK2/PPPP1PPP/R1BQ1B1R w - - 0 1");
    PrettyPrint(state);

    auto illegalMove = CreateMove(e3, f5);
    //checkLegal(state, illegalMove);

    state = StateFromFen("8/8/8/8/8/4p3/3K4/8 w - - 0 1");
    isCheck(state, true);

    return 0;
}