#include <iostream>
#include <iomanip>

unsigned long long Rotate180_(unsigned long long bb)
{
    //bb = __builtin_bswap64(bb);
    bb = ((bb >> 1) & 0x5555555555555555ULL) | ((bb & 0x5555555555555555ULL) << 1);
    bb = ((bb >> 2) & 0x3333333333333333ULL) | ((bb & 0x3333333333333333ULL) << 2);
    bb = ((bb >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((bb & 0x0F0F0F0F0F0F0F0FULL) << 4);
    bb = ((bb >> 8) & 0x00FF00FF00FF00FFULL) | ((bb & 0x00FF00FF00FF00FFULL) << 8);
    bb = ((bb >> 16) & 0x0000FFFF0000FFFFULL) | ((bb & 0x0000FFFF0000FFFFULL) << 16);
    bb = (bb >> 32) | (bb << 32);
    return bb;
}


int main()
{
    unsigned long long b = 0x8000000000000000ULL;
    std::cout << std::hex << std::setfill('0') << std::setw(16) << Rotate180_(b) << std::endl;
    return 0;
}