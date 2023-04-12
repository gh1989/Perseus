#include <numeric>
#include "geometry.h"
#include "move.h"
#include "move_generation.h"

/* Move generation */
size_t GenerateMoves(const State& state, Move * moves) {

	size_t numMoves = 0;

	/* Offset for moving/enemy bitboards */
	const uint8_t pieceStart = state.turn * NUMBER_PIECES;
	const uint8_t enemyPieceStart = (!state.turn) * NUMBER_PIECES;
	bool whitePieces = !state.turn;
	
	/* Occupation */
	auto bit_or = [&](const Bitboard &a, const Bitboard &b) { return a | b; };
	Bitboard moveOccupation = std::accumulate<>(state.bbs + pieceStart, state.bbs + pieceStart + NUMBER_PIECES, Bitboard(0), bit_or);
	Bitboard enemyOccupation = std::accumulate<>(state.bbs + enemyPieceStart, state.bbs + enemyPieceStart + NUMBER_PIECES, Bitboard(0), bit_or);
	
	/* Pawn moves */
	auto pawnBB = state.bbs[pieceStart + PAWN];
	size_t pawnMoves = PawnMoves(state, moves, pawnBB, moveOccupation, enemyOccupation, whitePieces);
	moves += pawnMoves;

	/* Knight */
	auto knightBB = state.bbs[pieceStart + KNIGHT];
	size_t knightMoves = JumperMoves<KNIGHT>(state, moves, knightBB, moveOccupation);
	moves += knightMoves;

	/* Bishop */
	auto queenBB = state.bbs[pieceStart + QUEEN];
	auto bishopBB = state.bbs[pieceStart + BISHOP];
	auto beeshopBB = queenBB | bishopBB;
	size_t bishopMoves = SliderMoves<BISHOP>(state, moves, beeshopBB, moveOccupation, enemyOccupation);
	moves += bishopMoves;

	/* Rook */
	Bitboard rookBB = state.bbs[pieceStart + ROOK];
	auto theRookBB = rookBB | queenBB;
	size_t rookMoves = SliderMoves<ROOK>(state, moves, theRookBB, moveOccupation, enemyOccupation);
	moves += rookMoves;

	/* King */
	auto kingBB = state.bbs[pieceStart + KING];
	size_t kingMoves = JumperMoves<KING>(state, moves, kingBB, moveOccupation);
	kingMoves += KingCastling(state, moves, kingBB, rookBB, moveOccupation, enemyOccupation, whitePieces);
	moves += kingMoves;
	
	return pawnMoves + kingMoves + rookMoves + bishopMoves + knightMoves;
}

size_t PawnMoves(
	const State& state, 
	Move* moves, 
	const Bitboard& pawnBB, 
	const Bitboard& moveOccupancy, 
	const Bitboard& enemyOccupancy, 
	bool whitePieces)
{
	/* Pawn moves: without rotation */
	size_t numMoves = 0;
	Bitboard square, push_once, push_twice, capt_diag;
	const size_t id_ep = 12;
			
	const Piece promote_pieces[4] = { KNIGHT, BISHOP, ROOK, QUEEN };

	int pawn_off_1    	 = whitePieces ?  8 :  -8;
	int pawn_off_2    	 = whitePieces ? 16 : -16;
	const int p_att_l 	 = whitePieces ?  7 :  -7;
	const int p_att_r  	 = whitePieces ?  9 :  -9;
	const auto cpattacks = whitePieces ? pawn_attacks_b : pawn_attacks;
	const int promRank   = whitePieces ? 6 : 1;
	const int dblPushRnk = whitePieces ? 1 : 6;

	auto fullOccupancy = moveOccupancy | enemyOccupancy;

	for (auto sqbb : BitboardRange(pawnBB)) {

		auto sqr = Square(sqbb);
		int rank = sqr / 8;
		const bool promote_cond = rank == promRank;
		square = squares[sqr];
		// Pushes
		push_once = squares[sqr + pawn_off_1];
		if (!(push_once & fullOccupancy))
		{
			// Double push.
			if (rank == dblPushRnk)
			{
				push_twice = squares[sqr + pawn_off_2];
				if (!(push_twice & fullOccupancy)) {
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

		if (cpattacks[sqr] & (enemyOccupancy | state.bbs[id_ep]))
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
				if (capt_diag & (enemyOccupancy | state.bbs[id_ep]))
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

	return numMoves;
}

size_t KingCastling(
	const State& state,
	Move *moves,
	const Bitboard& kingBB,
	const Bitboard& rookBB,
	const Bitboard& moveOccupation,
	const Bitboard& enemyOccupation,
	bool wPieces)
{
	size_t kingMoves = 0;
	auto kingStart  = wPieces ? e1 : e8;
	
	auto kCastleEnd = wPieces ? g1 : g8;
	auto ksCastling = wPieces ? Castling::WK : Castling::BK;
	auto kRookStart = wPieces ? h1 : h8;
	if ((rookBB & squares[kRookStart]) && (state.castle & ksCastling ))
	{
		*moves++ = CreateCastle(kingStart, kCastleEnd);
		kingMoves++;
	}

	auto qsCastling = wPieces ? Castling::WQ : Castling::BQ;
	auto qCastleEnd = wPieces ? c1 : c8;
	auto qRookStart = wPieces ? a1 : a8;
	if ((rookBB & squares[qRookStart]) && (state.castle & qsCastling ))
	{
		*moves++ = CreateCastle(kingStart, qCastleEnd);
		kingMoves++;
	}

	return kingMoves;
}

template <Piece _Piece>
size_t JumperMoves(
    const State& s,
    Move* moves,
	const Bitboard& pieceBB,
    const Bitboard& moveOccupation )
{
	auto attacks = knight_attacks;
	if(_Piece == KING)
		attacks = neighbours;

    size_t numMoves = 0;
    for (const auto& sqbb : BitboardRange(pieceBB))
    {
        Square sqr = Square(sqbb);
        Bitboard square = squares[sqr];
        const auto& attack = attacks[sqr] & (~moveOccupation);
        std::transform(BitboardRange(attack).begin(), BitboardRange(attack).end(), moves,
            [&](auto sqbb2) {
                return CreateMove(sqr, Square(sqbb2));
            });
        numMoves += attack.PopCnt();
        moves += attack.PopCnt();
    }

    return numMoves;
}

template <Piece _Piece>
inline std::size_t SliderMoves(
    const State& state,
    Move* moves,
    const Bitboard& sliderPieces,
    const Bitboard& moveOccupancy,
    const Bitboard& enemyOccupancy)
{
    std::size_t numMoves = 0;
	
	auto directions = bishop_directions;
	if(_Piece == ROOK)
		directions = rook_directions;

    for (auto sliderSquare : BitboardRange(sliderPieces))
    {
        Bitboard squareBB = squares[sliderSquare];
        for (const auto& dir : directions)
        {
            int dx = dir.first;
            int dy = dir.second;
            int sidx = sliderSquare;
            int rank = sidx / 8;
            for (; (dx > 0 && rank < 7) || (dx < 0 && rank > 0) || (dy > 0 && sidx % 8 < 7) || (dy < 0 && sidx % 8 > 0); sidx += 8 * dx + dy, rank += dx)
            {
                if (sidx < 0 || sidx > 63)
                    break;

                Bitboard newSquareBB = squares[sidx];

                if (newSquareBB & moveOccupancy)
                    break;

                *moves++ = CreateMove(Square(sliderSquare), Square(sidx));
                numMoves++;

                if (newSquareBB & enemyOccupancy)
                    break;
            }
        }
    }
    return numMoves;
}

bool isCheck(const State& state, bool whitePieces)
{
	/* Offset for bitboards */
	const uint8_t offsetUs = whitePieces ? 0 : NUMBER_PIECES;
	const uint8_t offsetThem = whitePieces ? NUMBER_PIECES : 0;

	Bitboard ourKing = state.bbs[offsetUs + KING];
	auto enemyKnights = state.bbs[offsetThem + KNIGHT];

	Square thisKingSquare = square_lookup.at(ourKing.bit_number);
	if ((knight_attacks[thisKingSquare] & enemyKnights).bit_number)
		return true;

	auto enemyPawnAttackMask = whitePieces ? pawn_attacks : pawn_attacks_b;
	auto enemyPawns = state.bbs[offsetThem + PAWN];
	if ((enemyPawnAttackMask[thisKingSquare] & enemyPawns).bit_number)
		return true;

	auto bit_or = [&](const Bitboard &a, const Bitboard &b) { return a | b; };
	Bitboard occupancy = std::accumulate<>(state.bbs, state.bbs + 2*NUMBER_PIECES, Bitboard(0), bit_or);

	auto queens = state.bbs[offsetThem + QUEEN];
	auto bishops = state.bbs[offsetThem + BISHOP];
	auto diagonals = queens | bishops;
	if (SquareConnectedToBitboard(thisKingSquare, diagonals, occupancy&~diagonals, bishop_directions))
		return true;

	auto rooks = state.bbs[offsetThem + ROOK];
	auto straights = queens | rooks;
	if (SquareConnectedToBitboard(thisKingSquare, straights, occupancy&~straights, rook_directions))
		return true;

	return false;
}