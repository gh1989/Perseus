#pragma once
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

	uint8_t nMSB() const {
		return std::bit_floor(bit_number);
	}

	unsigned int nLSB() const {
		return std::numeric_limits<decltype(bit_number)>::digits - std::bit_width(bit_number);
	}
	
	uint64_t msb64(uint64_t x) const {
		return std::countr_zero(x);
	}

	uint64_t bit_number;
};

/* Bitboard functions */
inline Bitboard Rotate180(Bitboard bb)
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
