#pragma once
#include "state.h"
#include <sstream>

/* Bitboard functions */
Bitboard Rotate180(Bitboard bb)
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

uint64_t _ByteSwap(uint64_t to_swap)
{
	return _byteswap_uint64(to_swap);
}

/* Square functions */
Square Reflect(Square sqr)
{
	int square_int = static_cast<int>(sqr);
	square_int = (square_int % 8) + (7 - (square_int / 8)) * 8;
	Square output = static_cast<Square>(square_int);
	return output;
}
Bitboard GetSquare(Square sqr) { return squares[sqr]; }
Bitboard BitMove(Bitboard bb, Square from, Square to) {
	return OnBit(OffBit(bb, from), to);
}
// Check if a square is on 
bool IsOn(Bitboard bb, Square square) {
	Bitboard s = SqrBb(square);
	return bool(bb & s);
}
// Turn a square off
Bitboard OffBit(Bitboard bb, Square off) {
	Bitboard s = SqrBb(off);
	return bb & (~s);
}
// Turn a square on
Bitboard OnBit(Bitboard bb, Square on) {
	Bitboard s = SqrBb(on);
	return bb | s;
}
template<class Iter, class T>
Iter binary_find(Iter begin, Iter end, T val) {
	// Finds the lower bound in at most log(last - first) + 1 comparisons
	Iter i = std::lower_bound(begin, end, val);

	if (i != end && !(val < *i))
		return i; // found
	else
		return end; // not found
}
// Convert bitboard to square
Square BbSqr(Bitboard bb){
	return static_cast<Square>(bb.nMSB());
	return square_lookup.at(bb);
}
// Convert square to bitboard
Bitboard SqrBb(Square sqr) {
	return squares[sqr];
}

/* Move functions */
Move CreateMove(Square from, Square to){
	return from + (to << 6);
}
Move CreatePromotion(Square from, Square to, Piece promo){
	return CreateMove(from, to) + (PROMOTE << flag_bits) + (promo << prom_bits);
}
Move CreateEnPassant(Square from, Square to){
	return CreateMove(from, to) + (ENPASSANT << flag_bits);
}

// From and to will be the king, this will give information
// about kingside/queenside and which king is castling.
Move CreateCastle(Square from, Square to){
	return CreateMove(from, to) + (CASTLE << flag_bits);
}
Square GetFrom(Move move){
	const __int16 sqr = from_mask & move;
	return Square(sqr);
}
Square GetTo(Move move){
	return Square((to_mask & move) >> to_bits);
}
SpecialMove SpecialMoveType(Move move){
	return SpecialMove((flag_mask & move) >> flag_bits);
}
Piece PromotionPiece(Move move){
	return Piece((prom_mask & move) >> prom_bits);
}
Move ReflectMove(Move move) {
	auto s = GetFrom(move);
	s = Reflect(s);

	auto f = GetTo(move);
	f = Reflect(f);

	return s + (f << to_bits) + ((flag_mask + prom_mask) & move);
}

/* UCI and string operations */
std::string AsUci(Move move){
	std::stringstream ss;
	ss << SquareName(GetFrom(move));
	ss << SquareName(GetTo(move));
	if (SpecialMoveType(move) == PROMOTE)
		ss << PieceStringLower(PromotionPiece(move));
	return  ss.str();
}
// Get square name as string
std::string SquareName(Square sqr)
{
	const std::string file_strings[8] = { "a", "b", "c", "d", "e", "f", "g", "h" };
	const std::string rank_strings[8] = { "1", "2", "3", "4", "5", "6", "7", "8" };
	int square_int = static_cast<int>(sqr);
	return file_strings[square_int % 8] + rank_strings[int(square_int / 8)];
}
std::string PieceStringLower(Piece piece) { return piece_strings[piece]; }

/* class: State */
State::State(std::string fen) {

}

void State::Apply(Move move) {
	auto specl = SpecialMoveType(move);
	bool new50 = false;
	bool white = 0;
	bool other = !white;
	auto start = GetFrom(move);
	auto finish = GetTo(move);

	const uint8_t ip	= NUMBER_PIECES * white + PAWN;
	const uint8_t ip2	= NUMBER_PIECES * other + PAWN;
	const uint8_t ik	= NUMBER_PIECES * white + KING;
	const uint8_t rk	= NUMBER_PIECES * white + ROOK;

	/* Removing captured pieces of opposition */
	uint8_t i_other;
	int p_other;
	Bitboard pbb_other;
	for (p_other = 0; p_other < NUMBER_PIECES; p_other++) {
		i_other = NUMBER_PIECES * other + p_other;
		pbb_other = bbs[i_other];
		if (IsOn(pbb_other, finish))
		{
			bbs[i_other] = OffBit(pbb_other, finish);
			new50 = true;
			break;
		}
	}
	
	/* Update en passant state, disable castling */
	int i, p;
	Bitboard pbb;
	for (p = 0; p < NUMBER_PIECES; p++) {
		i = NUMBER_PIECES * white + p;
		pbb = bbs[i];
		if (IsOn(pbb, start)) {
			new50 = (p == PAWN);
			if (p == PAWN && abs(finish - start) > 15) {
				bbs[12] = Bitboard(1ULL << (white ? (finish - 8) : (start - 8)));
			}
			else { bbs[12] = 0; }

			if (p == KING) {
				castle = castle & ~(white ? (WQ + WK) : (BQ + WK));
			}
			else if (p == ROOK) {
				if (start == a1) castle &= ~WQ;
				else if (start == h1) castle &= ~WK;
				else if (start == a8) castle &= ~BQ;
				else if (start == h8) castle &= ~BK;
			}
			bbs[i] = OffBit(pbb, start);
			break;
		}
	}
	
	/* Not special */
	if (specl == NONE) {
		bbs[i] = OnBit(pbb, finish);
	}
	/* En passant capture */
	if(specl==ENPASSANT) {
		new50 = true;
		auto passantp = white ? (bbs[12] >> 8) : (bbs[12] << 8);
		bbs[ip] = bbs[ip] | bbs[12];
		bbs[ip2] = bbs[ip2] & ~passantp;
	}
	/* Promotion */
	if (specl == PROMOTE) {
		new50 = true;
		auto prom = PromotionPiece(move);
		const uint8_t i_prom = NUMBER_PIECES*white + prom;
		auto our_promote_pieces = 
		bbs[i_prom] = OnBit( bbs[i_prom], finish);
	}
	/* Castling */
	if (specl == CASTLE) {
		bbs[ik] = OnBit( bbs[ik], finish);
		bool qs = (finish % 8) < 4;
		auto s1 = Square(qs ? finish - 2 : finish + 1);
		auto s2 = Square(qs ? finish + 1 : finish - 1);
		bbs[rk] = BitMove( bbs[rk], s1, s2 );
		castle = castle & ~(white ? (WQ + WK) : (BQ + WK));
	}
	c50 = new50 ? 0 : c50++;
}

/* Move generation */
/*
Move* generate_moves(const State& state, Move* moves) {
}
*/