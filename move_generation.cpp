#include "geometry.h"
#include "move.h"
#include "move_generation.h"

bool isCheck(const State& state, bool turn)
{
	/* Offset for bitboards */
	const uint8_t nn = turn * NUMBER_PIECES;
	const uint8_t not_nn = (!turn)*NUMBER_PIECES;

	Bitboard thisKing = state.bbs[nn + KING];
	auto otherKnights = state.bbs[not_nn + KNIGHT];

	for (auto it = thisKing.begin(); it != thisKing.end(); it.operator++())
	{
		auto thisKingSquare = Square(*it);
		if (knight_attacks[thisKingSquare] & otherKnights)
			return true;

		auto thisPawnAttacks = !turn ? pawn_attacks : pawn_attacks_b;
		auto otherPawns = state.bbs[not_nn + PAWN];
		if (thisPawnAttacks[thisKingSquare] & otherPawns)
			return true;

		auto bit_or = [&](const Bitboard &a, const Bitboard &b) { return a | b; };
		Bitboard occupancy = accumulate<>(state.bbs, state.bbs + 12, Bitboard(0), bit_or);

		auto queens = state.bbs[not_nn + QUEEN];
		auto bishops = state.bbs[not_nn + BISHOP];
		auto diagonals = queens | bishops;
		if (SquareConnectedToBitboard(thisKingSquare, diagonals, occupancy&~diagonals, bishop_directions))
			return true;

		auto rooks = state.bbs[not_nn + ROOK];
		auto straights = queens | rooks;
		if (SquareConnectedToBitboard(thisKingSquare, straights, occupancy&~straights, rook_directions))
			return true;
	}
	return false;
}

/* Move generation */
size_t GenerateMoves(const State& state, Move* moves) {

	size_t numMoves = 0;

	/* Offset for bitboards */
	const uint8_t nn = state.turn * NUMBER_PIECES;
	const uint8_t not_nn = (!state.turn)*NUMBER_PIECES;

	// All occupants of the state.
	Bitboard occ = 0;
	Bitboard occ_b = 0;
	Bitboard occ_w = 0;
	for (int i = 0; i < 12; i++)
	{
		auto tmp = state.bbs[i];
		occ = occ | tmp;
		if (i < 6)
			occ_w = occ_w | tmp;
		else
			occ_b = occ_b | tmp;
	};
	auto occ_other = state.turn ? occ_w : occ_b;

	/* Pawn moves: without rotation */
	const uint8_t ip = nn + PAWN;
	Bitboard square, push_once, push_twice, capt_diag;
	int pawn_off_1 = state.turn ? -8 : 8;
	int pawn_off_2 = state.turn ? -16 : 16;
	const int p_att_l = state.turn ? -7 : 7;
	const int p_att_r = state.turn ? -9 : 9;
	const size_t id_ep = 12;
	auto bb_pawn = state.bbs[ip];
	const auto cpattacks = !state.turn ? pawn_attacks : pawn_attacks_b;
	bool trn = state.turn;
	const Piece promote_pieces[4] = { KNIGHT, BISHOP, ROOK, QUEEN };
	for (auto it = bb_pawn.begin(); it != bb_pawn.end(); it.operator++()) {

		auto sqr = Square(*it);
		int rank = sqr / 8;
		const bool promote_cond = ((rank == 6)&(!trn) || (rank == 1)&trn);
		square = squares[sqr];
		// Pushes
		push_once = squares[sqr + pawn_off_1];
		if (!(push_once & occ))
		{
			// Double push.
			if ((rank == 1)&(!trn) || (rank == 6)&trn)
			{
				push_twice = squares[sqr + pawn_off_2];
				if (!(push_twice & occ)) {
					*moves++ = CreateMove(sqr, Square(sqr + pawn_off_2));
					numMoves++;
				}
			}
			if (promote_cond)
				for (auto& prom : promote_pieces) {
					*moves++ = CreatePromotion(sqr, Square(sqr + pawn_off_1), prom);
					numMoves++;
				}
			else {
				*moves++ = CreateMove(sqr, Square(sqr + pawn_off_1));
				numMoves++;
			}
		}

		if (cpattacks[sqr] & (occ_other | state.bbs[id_ep]))
			for (int shft : {p_att_l, p_att_r})
			{
				// You cannot capture off the side of the board.
				uint8_t file = sqr % 8;
				if (file == 0 && shft == p_att_l)
					continue;
				if (file == 7 && shft == p_att_r)
					continue;
				if ((shft < -sqr) || (shft > 63 - sqr))
					throw;

				capt_diag = shft < 0 ? square >> -shft : square << shft;
				auto to = Square(sqr + shft);
				if (capt_diag & (occ_other | state.bbs[id_ep]))
				{
					// Promotion
					if (promote_cond)
						for (auto& prom : promote_pieces)
						{
							*moves++ = CreatePromotion(sqr, to, prom);
							numMoves++;
						}
					else
					{
						// Taking enpassant
						if (cpattacks[sqr] & state.bbs[id_ep])
						{
							*moves++ = CreateEnPassant(sqr, to);
							numMoves++;
						}
						else
						{
							*moves++ = CreateMove(sqr, to);
							numMoves++;
						}
					}
				}
			}
	}

	auto bit_or = [&](const Bitboard &a, const Bitboard &b) { return a | b; };
	Bitboard current_occupation = accumulate<>(state.bbs + nn, state.bbs + nn + NUMBER_PIECES, Bitboard(0), bit_or);
	Bitboard other_occupation = accumulate<>(state.bbs + not_nn, state.bbs + not_nn + NUMBER_PIECES, Bitboard(0), bit_or);

	/* Knight */
	size_t knightMoves = JumperMoves(state, moves, KNIGHT, knight_attacks, current_occupation);
	moves += knightMoves;

	Bitboard queenbb = state.bbs[trn * NUMBER_PIECES + QUEEN];
	Bitboard rookbb = state.bbs[trn * NUMBER_PIECES + ROOK];
	Bitboard bishopbb = state.bbs[trn * NUMBER_PIECES + BISHOP];

	/* Bishop */
	size_t bishopMoves = SliderMoves(state, moves, queenbb | bishopbb, bishop_directions, current_occupation, other_occupation);
	moves += bishopMoves;

	/* Rook */
	size_t rookMoves = SliderMoves(state, moves, rookbb | queenbb, rook_directions, current_occupation, other_occupation);
	moves += rookMoves;

	/* King */
	size_t kingMoves = JumperMoves(state, moves, KING, neighbours, current_occupation);
	moves += kingMoves;

	// Add the castling

	auto kingbb = state.bbs[trn*NUMBER_PIECES + KING];
	if (kingbb & squares[!trn ? e1 : e8])
	{
		auto kingStart = !trn ? e1 : e8;
		auto queensideCastleEnd = !trn ? c1 : c8;
		auto kingsideCastleEnd = !trn ? g1 : g8;

		if ((rookbb & squares[!trn ? h1 : h8]) && (state.castle & ( trn ? Castling::BK : Castling::WK)))
		{
			*moves++ = CreateCastle(kingStart, kingsideCastleEnd);
			kingMoves++;
		}

		if ((rookbb & squares[!trn ? a1 : a8]) && (state.castle & (trn ? Castling::BK : Castling::WK)))
		{
			*moves++ = CreateCastle(kingStart, queensideCastleEnd);
			kingMoves++;
		}
	}

	numMoves += kingMoves + rookMoves + bishopMoves + knightMoves;

	return numMoves;
}

size_t JumperMoves(
	const State& s,
	Move* moves,
	Piece p,
	const Bitboard* attacks,
	Bitboard current_occupation)
{
	Bitboard piecebb = s.bbs[s.turn * NUMBER_PIECES + p];
	size_t numMoves = 0;
	for (auto it = piecebb.begin(); it != piecebb.end(); it.operator++())
	{
		Square sqr = Square(*it);
		;			Bitboard square = squares[sqr];
		auto attack = attacks[sqr] & (~current_occupation);
		for (auto it2 = attack.begin(); it2 != attack.end(); it2.operator++())
		{
			Square sqr2 = Square(*it2);
			auto move = CreateMove(Square(sqr), Square(sqr2));
			*moves++ = move;
			numMoves++;
		}
	}

	return numMoves;
}

inline size_t SliderMoves(
	const State& s,
	Move* moves,
	Bitboard piecebb,
	const std::pair<int, int>* directions,
	Bitboard current_occupation,
	Bitboard other_occupation)
{
	size_t numMoves = 0;
	for (auto it = piecebb.begin(); it != piecebb.end(); it.operator++())
	{
		Square sqr = Square(*it);
		Bitboard square = squares[sqr];

		for (int i = 0; i < 4; i++)
		{
			std::pair<int, int> dir = directions[i];
			auto dx = dir.first;
			auto dy = dir.second;
			int sidx = sqr;
			int rank = sidx / 8;
			while (true)
			{
				if (dx > 0 && rank == 7)
					break;
				if (dx < 0 && rank == 0)
					break;
				int file = sidx % 8;
				if (dy > 0 && file == 7)
					break;
				if (dy < 0 && file == 0)
					break;

				sidx += 8 * dx + dy;
				rank += dx;

				if ((sidx > 63) | (sidx < 0))
					break;
				Bitboard new_bb = squares[sidx];

				// slider hits own piece
				if (new_bb&current_occupation)
					break;
				auto sqrto = Square(sidx);
				*moves++ = CreateMove(sqr, sqrto);
				numMoves++;

				// slider hits opposition 
				if (new_bb&other_occupation)
					break;
			}
		}
	}

	return numMoves;
}