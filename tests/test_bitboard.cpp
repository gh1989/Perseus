#include <catch2/catch_all.hpp>
#include <iostream>
#include "../src/bitboard.h"

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
            unsigned int expected[4] = {0, 9, 18, 31};
            for (BitIterator it(bb, bb.nLSB()); it != BitIterator(0, 64); ++it) {
                REQUIRE(*it == expected[count]);
                count++;
            }
            REQUIRE(count == 4);
        }
    }

    TEST_CASE("Rotation of 0x8040201008040201ULL") {
        Bitboard bb = 0x8040201008040201ULL;
        Bitboard expected = 0x0102040810204080ULL;
        Bitboard result = Rotate180(bb);
        REQUIRE(result == expected);
    }

    TEST_CASE("Rotation of 0x8000000000000000ULL") {
        Bitboard bb = 0x8000000000000000ULL;
        Bitboard expected = 0x0000000000000080ULL;
        Bitboard result = Rotate180(bb);
        REQUIRE(result == expected);
    }

    TEST_CASE("Rotation of 0x0101010101010101ULL") {
        Bitboard bb = 0x0101010101010101ULL;
        Bitboard expected = 0x8080808080808080ULL;
        Bitboard result = Rotate180(bb);
        REQUIRE(result == expected);
    }

    TEST_CASE("Rotation of 0x55AA55AA55AA55AAULL") {
        Bitboard bb = 0x55AA55AA55AA55AAULL;
        Bitboard expected = 0xAA55AA55AA55AA55ULL;
        Bitboard result = Rotate180(bb);
        REQUIRE(result == expected);
    }

int main(int argc, char* argv[]) {
    // This line starts the Catch2 test runner
    return Catch::Session().run(argc, argv);
}