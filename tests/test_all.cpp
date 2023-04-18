#include <catch2/catch_all.hpp>
#include <iostream>

#include "../src/bitboard.h"
#include "../src/state.h"
#include "../src/string_transforms.h"
#include "../src/move_generation.h"

#define CATCH_CONFIG_MAIN

TEST_CASE("Empty board has no pieces set", "[bitboard]") {
    Bitboard empty_board;
    REQUIRE(empty_board.bit_number == 0ULL);
}

TEST_CASE("Bitwise or sets correct bits", "[bitboard]") {
    Bitboard board1(1ULL << 5);
    Bitboard board2(1ULL << 3);
    Bitboard result = board1 | board2;
    REQUIRE(result.bit_number == ((1ULL << 5) | (1ULL << 3)));
}

TEST_CASE("Bitwise and clears correct bits", "[bitboard]") {
    Bitboard board1(0xFF);
    Bitboard board2(0x0F);
    Bitboard result = board1 & board2;
    REQUIRE(result.bit_number == 0x0FUL);
}

TEST_CASE("Bitwise xor toggles correct bits", "[bitboard]") {
    Bitboard board1(0xFF);
    Bitboard board2(0x0F);
    Bitboard result = board1 ^ board2;
    REQUIRE(result.bit_number == 0xF0ULL);
}

TEST_CASE("Bitwise not inverts bits", "[bitboard]") {
    Bitboard board(0x3F);
    Bitboard result = ~board;
    REQUIRE(result.bit_number == ~(0x3FULL));
}

TEST_CASE("Bit shift left shifts bits", "[bitboard]") {
    Bitboard board(0x1);
    Bitboard result = board << 3;
    REQUIRE(result.bit_number == 0x8ULL);
}

TEST_CASE("Bit shift right shifts bits", "[bitboard]") {
    Bitboard board(0x80);
    Bitboard result = board >> 3;
    REQUIRE(result.bit_number == 0x10ULL);
}

TEST_CASE("Bitwise and assignment clears correct bits", "[bitboard]") {
    Bitboard board1(0xFF);
    Bitboard board2(0x0F);
    board1 &= board2;
    REQUIRE(board1.bit_number == 0x0FUL);
}

TEST_CASE("nMSB returns correct result for non-zero bitboards", "[bitboard]") {
    Bitboard board1(1ULL << 5);
    REQUIRE(board1.nMSB() == 5);

    Bitboard board2(1ULL << 3);
    REQUIRE(board2.nMSB() == 3);

    Bitboard board3(0xFF);
    REQUIRE(board3.nMSB() == 7);

    Bitboard board4(0x0102040800000000ULL);
    REQUIRE(board4.nMSB() == 56);
}

TEST_CASE("nMSB returns 0 for zero bitboard", "[bitboard]") {
    Bitboard empty_board;
    REQUIRE(empty_board.nMSB() == 0);
}

TEST_CASE("nLSB returns correct result for non-zero bitboards", "[bitboard]") {
    Bitboard board1(1ULL << 5);
    REQUIRE(board1.nLSB() == 5);

    Bitboard board2(1ULL << 3);
    REQUIRE(board2.nLSB() == 3);

    Bitboard board3(0xFF);
    REQUIRE(board3.nLSB() == 0);

    Bitboard board4(0x0102040800000000ULL);
    REQUIRE(board4.nLSB() == 35);
}

    TEST_CASE("nLSB returns 0 for zero bitboard", "[bitboard]") {
    Bitboard empty_board;
    REQUIRE(empty_board.nLSB() == 64);
    }

    TEST_CASE("BitIterator", "[BitIterator]") {
        SECTION("Empty bitboard") {
            Bitboard bb = 0;
            unsigned int count = 0;
            for (BitIterator it(bb, bb.nLSB()); it != BitIterator(0, 64); ++it) {
                count++;
            }
            REQUIRE(count == 0);
        }

        SECTION("Bitboard with one bit") {
            Bitboard bb = 0x8000000000000000ULL;
            unsigned int count = 0;
            for (BitIterator it(bb, bb.nLSB()); it != BitIterator(0, 64); ++it) {
                count++;
                REQUIRE(*it == 63);
            }
            REQUIRE(count == 1);
        }

        SECTION("Bitboard with multiple bits") {
            Bitboard bb = 0x0102040800000000ULL;
            unsigned int count = 0;
            unsigned int expected[4] = {35, 42, 49, 56};
            for (BitIterator it(bb.bit_number, bb.nLSB()); it != BitIterator(0, 64); ++it) {
                REQUIRE(*it == expected[count]);
                count++;
            }
            REQUIRE(count == 4);
        }
    }

    TEST_CASE("BitboardRange", "[BitboardRange]") {
    SECTION("Empty bitboard") {
        Bitboard bb = 0;
        unsigned int count = 0;
        for (unsigned int i : BitboardRange(bb)) {
            count++;
        }
        REQUIRE(count == 0);
    }

    SECTION("Bitboard with one bit") {
        Bitboard bb = 0x8000000000000000ULL;
        unsigned int count = 0;
        for (unsigned int i : BitboardRange(bb)) {
            count++;
            REQUIRE(i == 63);
        }
        REQUIRE(count == 1);
    }

    SECTION("Bitboard with multiple bits") {
        Bitboard bb = 0x0102040800000000ULL;
        unsigned int count = 0;
        unsigned int expected[4] = {35, 42, 49, 56};
        for (unsigned int i : BitboardRange(bb)) {
            REQUIRE(i == expected[count]);
            count++;
        }
        REQUIRE(count == 4);
    }

    SECTION("Full bitboard") {
        Bitboard bb = ~0ULL;
        unsigned int count = 0;
        for (unsigned int i : BitboardRange(bb)) {
            count++;
        }
        REQUIRE(count == 64);
    }

    SECTION("Castle path")
    {
        Bitboard bb = GetSquare(d8, c8);
        unsigned int count = 0;
        std::set expected{d8, c8};
        std::set<Square> result;
        for (unsigned int i : BitboardRange(bb)) {
            count++;
            result.insert(Square(i));
        }
        REQUIRE(count == 2);
        REQUIRE(expected == result);
    }
}

TEST_CASE("FEN to state conversion", "[state]")
{
    SECTION("Initial state")
    {
        std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        State state = StateFromFen(fen);
        REQUIRE(state.getBitboard(0).bit_number == 0x0000000000000042ULL);  // white knights
        REQUIRE(state.getBitboard(1).bit_number == 0x0000000000000024ULL);  // white bishops
        REQUIRE(state.getBitboard(2).bit_number == 0x0000000000000081ULL);  // white rooks
        REQUIRE(state.getBitboard(3).bit_number == 0x0000000000000008ULL);  // white queen
        REQUIRE(state.getBitboard(4).bit_number == 0x0000000000000010ULL);  // white king
        REQUIRE(state.getBitboard(5).bit_number == 0x000000000000FF00ULL);  // white pawns
        REQUIRE(state.getBitboard(6).bit_number == 0x4200000000000000ULL);  // black knights
        REQUIRE(state.getBitboard(7).bit_number == 0x2400000000000000ULL);  // black bishops
        REQUIRE(state.getBitboard(8).bit_number == 0x8100000000000000ULL);  // black rooks
        REQUIRE(state.getBitboard(9).bit_number == 0x0800000000000000ULL);  // black queen
        REQUIRE(state.getBitboard(10).bit_number == 0x1000000000000000ULL); // black king
        REQUIRE(state.getBitboard(11).bit_number == 0x00FF000000000000ULL); // black pawns
        REQUIRE(state.getCastleRights() == 15);  // all castling rights
        REQUIRE(state.isBlackMove() == 0);     // white to move
        REQUIRE(state.getEnPassant().bit_number == 0);  // no en passant square
        REQUIRE(state.getMoveCount() == 0);    
        REQUIRE(state.get50MoveCount() == 0);  
        REQUIRE(state.getMoveCount() == 0);    // first move
        REQUIRE(state.getPlies() == 0);
    }

    SECTION("Knight check state")
    {
        std::string fen = "rnbqkb1r/pppppppp/8/8/8/5n2/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        State state = StateFromFen(fen);

        REQUIRE(state.whitePiece<KING>().bit_number == 0x10ULL);  // white king
        REQUIRE( state.blackPiece<KNIGHT>().bit_number == 0x200000000200000ULL);  // black knights
    }

    SECTION("Check turn")
    {
        State state = StateFromFen("8/2k5/3P4/8/8/8/8/8 b - - 0 1");
        REQUIRE(state.isBlackMove() == 1);

        state = StateFromFen("8/2k5/3P4/8/8/8/8/8 w - - 0 1");
        REQUIRE(state.isBlackMove() == 0);
    }

    SECTION("Test enpassant")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
        REQUIRE(state.getEnPassant().bit_number == squares[e3].bit_number );
    }

    SECTION("Check c50")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 1");
        REQUIRE(state.get50MoveCount() == 23 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
        REQUIRE(state.get50MoveCount() == 0 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 100 1");
        REQUIRE(state.get50MoveCount() == 100 );
    }

    SECTION("Check plies")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 123");
        REQUIRE(int(state.getMoveCount()) == 122 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 13");
        REQUIRE(state.getMoveCount() == 12 );

        //state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 9999999");
        //REQUIRE(state.getMoveCount() == 9999998 );
    }
}

TEST_CASE("Knight bit boards from FEN strings", "[state]") {
  
    SECTION("Knight checkmate") {
        std::string fen = "7k/8/8/8/8/8/3N4/K7 w - - 0 1";
        State state = StateFromFen(fen);
        REQUIRE(state.whitePiece<KNIGHT>().bit_number  == 0x0000000000000800ULL);  // white knights
        REQUIRE(state.blackPiece<KING>().bit_number    == 0x8000000000000000ULL);  // black KING
        REQUIRE(state.whitePiece<KING>().bit_number  == 0x1ULL);  // white KING
    }

    SECTION("Knight stalemate") {
        std::string fen = "8/8/8/8/8/8/8/K1N5 w - - 0 1";
        State state = StateFromFen(fen);
        REQUIRE(state.whitePiece<KNIGHT>().bit_number == 0x0000000000000004ULL);  // white knights
        REQUIRE(state.whitePiece<KING>().bit_number   == 0x0000000000000001ULL);  // white king
    }
}

TEST_CASE("IsCheck", "[IsCheck]")
{
    SECTION("King in check")
    {
        State state = StateFromFen("rnbqkb1r/pppppppp/8/8/8/5n2/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        REQUIRE(isCheck(state, true) == true);

        state = StateFromFen("rnbqk1nr/pppppppp/8/8/1b1P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1");
        REQUIRE(isCheck(state, true) == true);

        state = StateFromFen("8/8/8/8/8/4p3/3K4/8 w - - 0 1");
        REQUIRE(isCheck(state, true) == true);
        
        state = StateFromFen("3r4/8/8/8/8/8/3K4/8 w - - 0 1");
        REQUIRE(isCheck(state, true) == true);
        
        state = StateFromFen("8/8/8/q7/8/8/3K4/8 w - - 0 1");
        REQUIRE(isCheck(state, true) == true);
        
        state = StateFromFen("8/8/8/3q4/8/8/3K4/8 w - - 0 1");
        REQUIRE(isCheck(state, true) == true);

        state = StateFromFen("8/2k5/3P4/8/8/8/8/8 b - - 0 1");
        REQUIRE(isCheck(state, false) == true);
    }

    SECTION("King not in check")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
        REQUIRE(isCheck(state, true) == false);

        state = StateFromFen("rnbqk1nr/pppppppp/8/8/1b6/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        REQUIRE(isCheck(state, true) == false);
        
        state = StateFromFen("8/8/8/3q4/8/3P4/3K4/8 w - - 0 1");
        REQUIRE(isCheck(state, true) == false);
        
        state = StateFromFen("8/8/8/q7/1n6/3P4/3K4/8 w - - 0 1");
        REQUIRE(isCheck(state, true) == false);
    }
}

TEST_CASE("Slider move generation", "[move_generation]") {
    State state = StateFromFen("8/8/8/8/8/8/K2pR2P/R7 w - - 0 1");
	TMoveContainer moves;

    SECTION("Rook move generation") {
        auto rookBB = state.whitePiece<ROOK>();
        
        // Generate moves for the white rook on a1
         SliderMoves<ROOK>(state, moves);

        std::set<Move> expectedMoves = {
            CreateMove(e2, d2), CreateMove(e2, f2), CreateMove(e2, g2), CreateMove(e2, e1), CreateMove(e2, e3), CreateMove(e2, e4),
            CreateMove(e2, e5), CreateMove(e2, e6), CreateMove(e2, e7), CreateMove(e2, e8), 
            CreateMove(a1, b1), CreateMove(a1, c1), CreateMove(a1, d1), CreateMove(a1, e1), CreateMove(a1, f1), CreateMove(a1, g1), CreateMove(a1, h1)
        };

        std::set<Move> movesGenerated = std::set( moves.begin(), moves.end() );
        REQUIRE( movesGenerated == expectedMoves );
    }

    SECTION("Bishop move generation") {
        std::string fenString = "rnbqkbnr/pppppppp/8/8/4B3/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        auto state = StateFromFen(fenString);

	    TMoveContainer moves;

        // Generate moves for the white bishop on e4
        SliderMoves<BISHOP>(state, moves);

        std::set<Move> expectedMoves = {
            CreateMove(e4, f5), CreateMove(e4, g6), CreateMove(e4, h7), CreateMove(e4, d3),
            CreateMove(e4, f3), CreateMove(e4, d5), CreateMove(e4, c6), CreateMove(e4, b7),
        };

        std::set<Move> movesGenerated = std::set(moves.begin(), moves.end());
        REQUIRE(movesGenerated == expectedMoves);
    }

    SECTION("Test king castling")
    {
        auto state = StateFromFen("rnbqkbnr/pppppppp/8/8/4B3/8/PPPPPPPP/R3K2R w KQkq - 0 1");
	    TMoveContainer moves;

        // Generate castling moves
        KingCastling(state, moves);

        std::set<Move> expectedMoves = {
            CreateMove(e1, g1), CreateMove(e1, c1)
        };

        std::set<Move> movesGenerated = std::set(moves.begin(), moves.end());
        REQUIRE(movesGenerated == expectedMoves);

        state = StateFromFen("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
        KingCastling(state, moves);

        expectedMoves = {
            CreateMove(e8, g8), CreateMove(e8, c8)
        };
    }
}

TEST_CASE("Test move application", "[moveApplication]")
{

    SECTION("Game from start position, castling, capturing, promotion")
    {
            State state = StateFromFen();

            // apply some moves
            state.Apply(CreateMove(e2, e4)); // white pawn moves 2 squares
            REQUIRE(state.isBlackMove());
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

            auto expectedState = StateFromFen("2kr1bRr/ppp4p/2n5/5bp1/2Bp2q1/6P1/PPP2P1P/RNBQ1RK1 b - - 0 11");

            for(int i=0; i<NUMBER_PIECES+1; i++)
                REQUIRE(state.getBitboard(i) == expectedState.getBitboard(i));
            REQUIRE(state.getEnPassant() == expectedState.getEnPassant());
            REQUIRE(state.getPlies() == expectedState.getPlies());
            REQUIRE(state.getCastleRights() == expectedState.getCastleRights());
            REQUIRE(state.getMoveCount() == expectedState.getMoveCount());
            REQUIRE(state.get50MoveCount() == expectedState.get50MoveCount());
            REQUIRE(state.isBlackMove() == expectedState.isBlackMove());
            
    }
}

TEST_CASE("checkLegal returns true for legal move", "[checkLegal]") {
    State state;
    Move illegalMove;
    
    SECTION("checkLegal diagonals")
    {
        state = StateFromFen("rnb1kbnr/pppppppp/8/8/1q6/8/PPPPPPPP/RNBQKBNR w - - 0 1");
        illegalMove = CreateMove(d2, d4);
        REQUIRE(checkLegal(state, illegalMove) == false);

    }

    SECTION("isCheck bug")
    {
        state = StateFromFen("1nb1kbnr/pppppppp/8/4P3/1r3K2/8/PPPP1PPP/RNBQ1BNR b - - 0 1");
        REQUIRE(isCheck(state, true));

        state = StateFromFen("1nb1kbnr/pppppppp/5r2/8/5K2/4N3/PPPP1PPP/R1BQ1B1R b - - 0 1");
        REQUIRE(isCheck(state, true));

    }

    SECTION("checkLegal ranks")
    {
        state = StateFromFen("1nb1k1nr/pppppppp/3b4/4P3/5K2/8/PPPP1PPP/RNBQ1BNR w - - 0 1");
        illegalMove = CreateMove(e5, e6);
        REQUIRE(checkLegal(state, illegalMove) == false);

        state = StateFromFen("1nb1kbnr/pppppppp/8/8/1r2PK2/8/PPPP1PPP/RNBQ1BNR w - - 0 1");
        illegalMove = CreateMove(e4, e5);
        REQUIRE(checkLegal(state, illegalMove) == false);

        state = StateFromFen("1nb1kbnr/pppppppp/8/8/8/2r1NK2/PPPP1PPP/R1BQ1B1R w - - 0 1");
        illegalMove = CreateMove(e3, f5);
        REQUIRE(checkLegal(state, illegalMove) == false);
    }

    SECTION("checkLegal files")
    {
        state = StateFromFen("1nb1kbnr/pppppppp/5r2/5N2/5K2/8/PPPP1PPP/R1BQ1B1R w - - 0 1");
        illegalMove = CreateMove(f5, e3);
        REQUIRE(checkLegal(state, illegalMove) == false);
    }

    SECTION("Cant castle when in check")
    {
        state = StateFromFen("1nb1kb1r/pppppppp/4r3/2n5/8/5B2/PPP2PPP/R1B1K2R w - - 0 1");
        illegalMove = CreateCastle(e1, g1);
        REQUIRE(checkLegal(state, illegalMove) == false );
    }

    SECTION("Cant castle through check")
    {
        state = StateFromFen("r3kb1r/ppp1pppp/8/2nb1B2/5pn1/8/PPP2PPP/R1BRK3 b - - 0 1");
        illegalMove = CreateCastle(e8, c8);
        REQUIRE(checkLegal(state, illegalMove) == false );
    }

    SECTION("Is bitboard attacked")
    {
        state = StateFromFen("r3kb1r/ppp1pppp/8/2nb1B2/5pn1/8/PPP2PPP/R1BRK3 b - - 0 1");
        Bitboard kingsPath = 0xc00000000000000ULL;
        kingsPath = GetSquare(d8, c8);
        REQUIRE( isBitboardAttacked(kingsPath, state, state.isBlackMove()) == true );
    }
}

int main(int argc, char* argv[]) {
    // This line starts the Catch2 test runner
    return Catch::Session().run(argc, argv);
}