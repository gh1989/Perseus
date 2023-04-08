#include <catch2/catch_all.hpp>
#include <iostream>
#include "../src/bitboard.h"
#include "../src/state.h"
#include "../src/string_transforms.h"

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
        /*
        State state = StateFromFen(fen);
        REQUIRE(state.bbs[0] == 0x0000000000000010ULL);  // white knights
        REQUIRE(state.bbs[1] == 0x0000000000000024ULL);  // white bishops
        REQUIRE(state.bbs[2] == 0x0000000000000081ULL);  // white rooks
        REQUIRE(state.bbs[3] == 0x0000000000000008ULL);  // white queen
        REQUIRE(state.bbs[4] == 0x0000000000000010ULL);  // white king
        REQUIRE(state.bbs[5] == 0x000000000000FF00ULL);  // white pawns
        REQUIRE(state.bbs[6] == 0x0000000000001000ULL);  // black knights
        REQUIRE(state.bbs[7] == 0x0000000000000042ULL);  // black bishops
        REQUIRE(state.bbs[8] == 0x8100000000000000ULL);  // black rooks
        REQUIRE(state.bbs[9] == 0x0800000000000000ULL);  // black queen
        REQUIRE(state.bbs[10] == 0x1000000000000000ULL); // black king
        REQUIRE(state.bbs[11] == 0x00FF000000000000ULL); // black pawns
        REQUIRE(state.castle == 15);  // all castling rights
        REQUIRE(state.turn == 0);     // white to move
        REQUIRE(state.bbs[12] == 0);  // no en passant square
        REQUIRE(state.c50 == 0);      // 50-move rule not applicable
        REQUIRE(state.plies == 0);    // first move
        */
    }
}

int main(int argc, char* argv[]) {
    // This line starts the Catch2 test runner
    return Catch::Session().run(argc, argv);
}