#pragma once
#include "bitboard.h"
#include "move.h"
/* class: State */
class State
{
public:
	State() = default;
	void Apply(Move);
	bool GetTurn() const { return turn; }

	Bitboard	bbs[13];
	uint8_t		c50;
	uint8_t		castle;
	uint32_t	plies;
	bool		turn;
};

std::uint64_t hash_combine(uint64_t lhs, uint64_t rhs);