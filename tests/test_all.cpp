#include <catch2/catch_all.hpp>
#include <iostream>
#include <numeric>

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
}

TEST_CASE("FEN to state conversion", "[state]")
{
    SECTION("Initial state")
    {
        std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        State state = StateFromFen(fen);
        REQUIRE(state.bbs[0].bit_number == 0x0000000000000042ULL);  // white knights
        REQUIRE(state.bbs[1].bit_number == 0x0000000000000024ULL);  // white bishops
        REQUIRE(state.bbs[2].bit_number == 0x0000000000000081ULL);  // white rooks
        REQUIRE(state.bbs[3].bit_number == 0x0000000000000008ULL);  // white queen
        REQUIRE(state.bbs[4].bit_number == 0x0000000000000010ULL);  // white king
        REQUIRE(state.bbs[5].bit_number == 0x000000000000FF00ULL);  // white pawns
        REQUIRE(state.bbs[6].bit_number == 0x4200000000000000ULL);  // black knights
        REQUIRE(state.bbs[7].bit_number == 0x2400000000000000ULL);  // black bishops
        REQUIRE(state.bbs[8].bit_number == 0x8100000000000000ULL);  // black rooks
        REQUIRE(state.bbs[9].bit_number == 0x0800000000000000ULL);  // black queen
        REQUIRE(state.bbs[10].bit_number == 0x1000000000000000ULL); // black king
        REQUIRE(state.bbs[11].bit_number == 0x00FF000000000000ULL); // black pawns
        //REQUIRE(state.castle == 15);  // all castling rights
        REQUIRE(state.turn == 0);     // white to move
        REQUIRE(state.bbs[12].bit_number == 0);  // no en passant square
        REQUIRE(state.c50 == 0);      // 50-move rule not applicable
        //REQUIRE(state.plies == 0);    // first move
    }

    SECTION("Knight check state")
    {
        std::string fen = "rnbqkb1r/pppppppp/8/8/8/5n2/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        State state = StateFromFen(fen);

        REQUIRE(state.bbs[KING].bit_number == 0x10ULL);  // white king
        REQUIRE( state.bbs[NUMBER_PIECES+KNIGHT].bit_number == 0x200000000200000ULL);  // black knights
    }

    SECTION("Check turn")
    {
        State state = StateFromFen("8/2k5/3P4/8/8/8/8/8 b - - 0 1");
        REQUIRE(state.turn == 1);

        state = StateFromFen("8/2k5/3P4/8/8/8/8/8 w - - 0 1");
        REQUIRE(state.turn == 0);
    }

    SECTION("Test enpassant")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
        REQUIRE(state.bbs[12].bit_number == squares[e3].bit_number );
    }

    SECTION("Check c50")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 1");
        REQUIRE(state.c50 == 23 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 0 1");
        REQUIRE(state.c50 == 0 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 100 1");
        REQUIRE(state.c50 == 100 );
    }

    SECTION("Check plies")
    {
        State state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 123");
        REQUIRE(state.plies == 123 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 13");
        REQUIRE(state.plies == 13 );

        state = StateFromFen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e3 23 9999999");
        REQUIRE(state.plies == 9999999 );
    }
}

TEST_CASE("Knight bit boards from FEN strings", "[state]") {
  
    SECTION("Knight checkmate") {
        std::string fen = "7k/8/8/8/8/8/3N4/K7 w - - 0 1";
        State state = StateFromFen(fen);
        REQUIRE(state.bbs[KNIGHT].bit_number                == 0x0000000000000800ULL);  // white knights
        REQUIRE(state.bbs[NUMBER_PIECES+KING].bit_number    == 0x8000000000000000ULL);  // black KING
        REQUIRE(state.bbs[KNIGHT].bit_number  == 0x0000000000000800ULL);  // black knights
    }

    SECTION("Knight stalemate") {
        std::string fen = "8/8/8/8/8/8/8/K1N5 w - - 0 1";
        State state = StateFromFen(fen);
        REQUIRE(state.bbs[KNIGHT].bit_number == 0x0000000000000004ULL);  // white knights
        REQUIRE(state.bbs[KING].bit_number   == 0x0000000000000001ULL);  // white king
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

	auto bit_or = [&](const Bitboard &a, const Bitboard &b) { return a | b; };
	Bitboard moveOccupation = std::accumulate<>(state.bbs, state.bbs + NUMBER_PIECES, Bitboard(0), bit_or);
	Bitboard enemyOccupation = std::accumulate<>(state.bbs + NUMBER_PIECES, state.bbs + 2 * NUMBER_PIECES, Bitboard(0), bit_or);
	TMoveContainer moves;

    SECTION("Rook move generation") {
        auto rookBB = state.bbs[ROOK];
        REQUIRE( rookBB.bit_number == 0x1001ULL );
        
        // Generate moves for the white rook on a1
         SliderMoves<ROOK>(state, moves, state.bbs[ROOK], moveOccupation, enemyOccupation);

        Move expectedMoves[] = {
            CreateMove(e2, d2), CreateMove(e2, f2), CreateMove(e2, g2), CreateMove(e2, e1), CreateMove(e2, e3), CreateMove(e2, e4),
            CreateMove(e2, e5), CreateMove(e2, e6), CreateMove(e2, e7), CreateMove(e2, e8), 
            CreateMove(a1, b1), CreateMove(a1, c1), CreateMove(a1, d1), CreateMove(a1, e1), CreateMove(a1, f1), CreateMove(a1, g1), CreateMove(a1, h1)
        };
/*
        for (int i = 0; i < 17; i++) {
            auto foundMove = std::find(std::begin(expectedMoves), std::end(expectedMoves), moves[i]) != std::end(expectedMoves);
            REQUIRE( foundMove );
            //std::cout << AsUci(foundMove) << std::endl;
        }

        std::cout << numMoves << std::endl;
        REQUIRE(numMoves == 17);
        */
    }
}


int main(int argc, char* argv[]) {
    // This line starts the Catch2 test runner
    return Catch::Session().run(argc, argv);
}