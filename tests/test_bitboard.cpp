#include <catch2/catch_all.hpp>
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

TEST_CASE("Bitboard iterator iteration works", "[bitboard]") {
    Bitboard board(0xA5);
    unsigned int count = 0;
    for (auto it : BitboardRange(board)) {
        ++count;
    }
    REQUIRE(count == 4);
}

int main(int argc, char* argv[]) {
    // This line starts the Catch2 test runner
    return Catch::Session().run(argc, argv);
}