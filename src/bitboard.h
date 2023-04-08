#pragma once
#include <limits>
#include <stdint.h>
#include <stdlib.h>
#include <bit>
#include <iterator> 

/* 	class: BitBoard */
struct Bitboard
{
	Bitboard() : bit_number(0) {}
	Bitboard(uint64_t bit_number_) : bit_number(bit_number_) {}

	bool operator==(const Bitboard& other) const {
		return bit_number == other.bit_number;
	}

	Bitboard operator<<(int x) const {
		return Bitboard(bit_number << x);
	}

	Bitboard operator~() const {
		return Bitboard(~bit_number);
	}

	Bitboard operator>>(int x) const {
		return Bitboard(bit_number >> x);
	}

	Bitboard operator| (const Bitboard& other) const {
		return Bitboard(bit_number | other.bit_number);
	}

	Bitboard operator& (const Bitboard& other) const {
		return Bitboard(bit_number & other.bit_number);
	}

	Bitboard operator^ (const Bitboard& other) const {
		return Bitboard(bit_number ^ other.bit_number);
	}

	Bitboard operator- (int value) const {
		return bit_number - value;
	}

	void operator &= (const Bitboard& other) {
		bit_number &= other.bit_number;
	}

	operator bool() const {
		return bit_number != 0;
	}

	uint64_t PopCnt() const {
		return std::popcount(bit_number);
	}

	uint8_t nMSB()
	{
		// If the value is 0, return 0 (the position of the 0th bit)
		if (bit_number == 0) {
			return 0;
		}
		
		// Use __builtin_clz to count the number of leading zeros in the value
		// Subtract the number of leading zeros from the number of bits in the value's type
		return std::numeric_limits<uint64_t>::digits - __builtin_clzll(bit_number) - 1;
	}
	uint8_t nLSB() const {
		// If the value is 0, return 64
		if (bit_number == 0) {
			return 64;
		}

		return __builtin_ctzll(bit_number);
	}

	uint64_t bit_number;
};

/* Bitboard functions */
inline Bitboard Rotate180(Bitboard bb)
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

/* class: BitIterator */
class BitIterator {
public:
    BitIterator(uint64_t value, uint8_t index) : value_(value), index_(index) {};
    bool operator!=(const BitIterator& other) const {
        return value_ != other.value_ || index_ != other.index_;
    }
    void operator++() {
        value_ &= (value_ - 1);
        index_ = value_.nLSB();
    }
    unsigned int operator*() const { return index_; }
private:
    Bitboard value_;
    uint8_t index_;
};

/* create a BitIterator range for a Bitboard */
class BitboardRange {
public:
    BitboardRange(Bitboard bb) : bb_(bb) {};
    BitIterator begin() const { return BitIterator(bb_, bb_.nLSB()); }
    BitIterator end() const { return BitIterator(0, 64); }
private:
    Bitboard bb_;
};
