#include <iostream>
#include <chrono>
#include "bitboard.h"

inline Bitboard Rotate180BuiltIn(Bitboard bb)
{
    bb.bit_number = __builtin_bswap64(bb.bit_number);
    bb.bit_number = ((bb.bit_number >> 1) & 0x5555555555555555) | ((bb.bit_number & 0x5555555555555555) << 1);
    bb.bit_number = ((bb.bit_number >> 2) & 0x3333333333333333) | ((bb.bit_number & 0x3333333333333333) << 2);
    bb.bit_number = ((bb.bit_number >> 4) & 0x0F0F0F0F0F0F0F0F) | ((bb.bit_number & 0x0F0F0F0F0F0F0F0F) << 4);
    bb.bit_number = ((bb.bit_number >> 8) & 0x00FF00FF00FF00FF) | ((bb.bit_number & 0x00FF00FF00FF00FF) << 8);
    bb.bit_number = ((bb.bit_number >> 16) & 0x0000FFFF0000FFFF) | ((bb.bit_number & 0x0000FFFF0000FFFF) << 16);
    bb.bit_number = (bb.bit_number >> 32) | (bb.bit_number << 32);
    return bb;
}

inline Bitboard Rotate180_(Bitboard bb)
{
	const Bitboard h1 = 0x5555555555555555;
	const Bitboard h2 = 0x3333333333333333;
	const Bitboard h4 = 0x0F0F0F0F0F0F0F0F;
	const Bitboard v1 = 0x00FF00FF00FF00FF;
	const Bitboard v2 = 0x0000FFFF0000FFFF;
	const Bitboard m1 = (0xFFFFFFFFFFFFFFFF);

	bb = ((bb >> 1) & h1) | ((bb & h1) << 1);
	bb = ((bb >> 2) & h2) | ((bb & h2) << 2);
	bb = ((bb >> 4) & h4) | ((bb & h4) << 4);
	bb = ((bb >> 8) & v1) | ((bb & v1) << 8);
	bb = ((bb >> 16) & v2) | ((bb & v2) << 16);
	bb = (bb >> 32) | (bb << 32);
	return bb & m1;
}

int main()
{
    const int num_iterations = 10000000;
    Bitboard bb = 0xF0F0F0F0F0F0F0FULL;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_iterations; i++)
    {
        bb = Rotate180_(bb);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Rotate180: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns\n";

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_iterations; i++)
    {
        bb = Rotate180BuiltIn(bb);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "Rotate180BuiltIn: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns\n";

    return 0;
}