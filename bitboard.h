#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <intrin.h>
#include <iterator>    

/* 	class: BitIterator */
template<class T>
class BitIterator {
public:
	BitIterator(uint64_t value) : value_(value) {};
	bool operator!=(const BitIterator& other) {
		return value_ != other.value_;
	}
	void operator++() { value_ &= (value_ - 1); }
	unsigned int operator*() const { return value_.nLSB(); }

private:
	T value_;
};

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
		return __popcnt64(bit_number);
	}

	uint8_t nMSB() const {
		uint64_t msb_val = msb64(bit_number);
		uint8_t msb_num = 0;
		while (msb_val > 1)
		{
			msb_num++;
			msb_val >>= 1;
		}
		return msb_num;
	}

	unsigned int nLSB() const {
		unsigned long result;
		_BitScanForward64(&result, bit_number);
		return result;
	}

	BitIterator<Bitboard> begin() {
		return bit_number;
	}

	BitIterator<Bitboard> end() {
		return 0;
	}
	uint64_t msb64(register uint64_t x) const {
		x |= (x >> 1);
		x |= (x >> 2);
		x |= (x >> 4);
		x |= (x >> 8);
		x |= (x >> 16);
		x |= (x >> 32);
		return(x & ~(x >> 1));
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

inline uint64_t _ByteSwap(uint64_t to_swap)
{
	return _byteswap_uint64(to_swap);
}