#include "catch.hpp"
#include "bitboard.h"

TEST_CASE("BitBoard tests", "[BitBoard]") {

    SECTION("Empty board has no pieces set") {
        BitBoard empty_board;
        REQUIRE(empty_board.value() == 0ULL);
    }

    SECTION("Setting a bit sets the corresponding piece") {
        BitBoard board;
        board.set(1);
        REQUIRE(board.value() == 2ULL);
    }

    SECTION("Clearing a bit clears the corresponding piece") {
        BitBoard board;
        board.set(1);
        board.clear(1);
        REQUIRE(board.value() == 0ULL);
    }

    SECTION("Bitwise operations work as expected") {
        BitBoard board1(3);
        BitBoard board2(6);
        REQUIRE((board1 & board2).value() == 2ULL);
        REQUIRE((board1 | board2).value() == 7ULL);
        REQUIRE((board1 ^ board2).value() == 5ULL);
    }

    SECTION("Bitwise shift operations work as expected") {
        BitBoard board(1);
        board <<= 2;
        REQUIRE(board.value() == 4ULL);
        board >>= 1;
        REQUIRE(board.value() == 2ULL);
    }
}
