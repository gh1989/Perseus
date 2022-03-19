#pragma once
#include "state.h"
#include <functional>
#include <iostream>
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

Bitboard BitboardFromString(std::string str)
{
	if (str[0] < 'a' || str[1] < '1' || str[0] > 'h' || str[1] > '8')
		throw std::runtime_error("Square string is formatted improperly.");
	uint64_t boardnum = str[0] - 'a' + 8 * (str[1] - '1');
	return Bitboard(1ULL << boardnum);
}

/* class: State */
State::State(std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {

	const std::string castle_str = "QKqk";

	std::string strings[6];
	std::istringstream f(fen);
	std::string s;
	int strCount = 0;
	while (std::getline(f, s, ' ')) {
		strings[strCount] = s;
		strCount++;
		if (strCount == 6) break;
	}
	
	if (strCount != 6)
		throw std::runtime_error( "Bad FEN" );
	
	auto posstring		= strings[0];
	auto colour_string	= strings[1];
	auto castlestring	= strings[2];
	auto epstring		= strings[3];
	auto fiftycounter	= strings[4];
	auto moveclock		= strings[5];

	auto ite = 8;
	size_t sub_ite, found;

	std::istringstream f2(posstring);
	while (std::getline(f2, s, '/')) {
		sub_ite = 0;
		for (char& c : s) {
			if (c >= '1' && c <= '9')
			{
				sub_ite += c - '0';
			}
			else
			{
				const std::string piece_strings = "NBRQKP";
				found = piece_strings.find(::toupper(c));
				if (found == std::string::npos)
					throw std::runtime_error("Bad FEN");
				auto piece = static_cast<Piece>(found);
				auto this_piece_colour = (::toupper(c) != c);
				auto square = static_cast<Square>(8 * (ite - 1) + sub_ite);
				uint8_t pid = this_piece_colour*NUMBER_PIECES + piece;
				bbs[pid] = OnBit( bbs[pid], square );
				sub_ite++;
			}
		}
		if (sub_ite != 8)
			throw std::runtime_error( "Bad FEN" );
		
		ite -= 1;
	}

	if (ite != 0)
		throw std::runtime_error( "Bad FEN" );

	if (castlestring != "-") {
		uint8_t castlesum = 0;
		for (char c : castlestring) {
			found = castle_str.find(c);
			if (found != std::string::npos)
				castlesum += 1 << found;
		}
		castle = castlesum;
	}
	else
		castle = 0;
	
	// 0 for w, 1 for b.
	turn = (colour_string != "w");
	
	if (epstring != "-")
		bbs[13] = BitboardFromString(epstring);
	else
		bbs[13] = 0;
	
	if ( fiftycounter != "-" ) {
		uint8_t counter = std::stoi(fiftycounter);
		if ( counter >= 999 )
			throw std::runtime_error( "Bad FEN" );
		c50 = counter;
	}
	else
		c50 = 0;
	
	if (moveclock != "-") {
		unsigned short counter = (std::stoi(moveclock) - 1);
		if ( counter > 999 )
			throw std::runtime_error("Bad FEN");
		plies = 2 * counter + turn;
	}
}

void State::Apply(Move move) {
	auto specl = SpecialMoveType(move);
	bool new50 = false;
	bool other = !turn;
	auto start = GetFrom(move);
	auto finish = GetTo(move);

	const uint8_t ip	= NUMBER_PIECES * turn + PAWN;
	const uint8_t ip2	= NUMBER_PIECES * other + PAWN;
	const uint8_t ik	= NUMBER_PIECES * turn + KING;
	const uint8_t rk	= NUMBER_PIECES * turn + ROOK;

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
		i = NUMBER_PIECES * turn + p;
		pbb = bbs[i];
		if (IsOn(pbb, start)) {
			new50 = (p == PAWN);
			if (p == PAWN && abs(finish - start) > 15) {
				bbs[12] = Bitboard(1ULL << (turn ? (finish - 8) : (start - 8)));
			}
			else { bbs[12] = 0; }

			if (p == KING) {
				castle = castle & ~(turn ? (WQ + WK) : (BQ + WK));
			}
			else if (p == ROOK) {
				if (start == a1) castle &= ~WQ;
				else if (start == h1) castle &= ~WK;
				else if (start == a8) castle &= ~BQ;
				else if (start == h8) castle &= ~BK;
			}
			bbs[i] = OffBit(bbs[i], start);
			break;
		}
	}
	
	/* Not special */
	if (specl == NONE) {
		bbs[i] = OnBit(bbs[i], finish);
	}
	/* En passant capture */
	if(specl==ENPASSANT) {
		new50 = true;
		auto passantp = turn ? (bbs[12] >> 8) : (bbs[12] << 8);
		bbs[ip] = bbs[ip] | bbs[12];
		bbs[ip2] = bbs[ip2] & ~passantp;
	}
	/* Promotion */
	if (specl == PROMOTE) {
		new50 = true;
		auto prom = PromotionPiece(move);
		const uint8_t i_prom = NUMBER_PIECES*turn + prom;
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
		castle = castle & ~(turn ? (WQ + WK) : (BQ + WK));
	}
	c50 = new50 ? 0 : c50++;
	turn = other;
}

bool isCheck(const State& state)
{
	/* Offset for bitboards */
	const uint8_t nn = state.turn * NUMBER_PIECES;
	const uint8_t not_nn = (!state.turn)*NUMBER_PIECES;

	Bitboard thisKing = state.bbs[nn + KING];
	auto otherKnights = state.bbs[not_nn + KNIGHT];
	Square thisKingSquare = square_lookup.at( static_cast<unsigned long long>(thisKing) );
	if (knight_attacks[thisKingSquare] & otherKnights)
		return true;

	auto thisPawnAttacks = state.turn ? pawn_attacks : pawn_attacks_b;
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
	
    for (auto it = bb_pawn.begin(); it != bb_pawn.end(); it.operator++()) {
		
		auto sqr = Square(*it);
		int rank = sqr / 8;
		const bool promote_cond = ((rank == 6)&(!trn) || (rank == 1)&trn);
		square = squares[sqr];
		// Pushes
		push_once = squares[sqr+pawn_off_1];
		if (!(push_once & occ))
		{
			// Double push.
			if ( (rank == 1)&(!trn) || (rank == 6)&trn )
			{
				push_twice = squares[sqr+ pawn_off_2];
				if (!(push_twice & occ)){
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
			if ((shft < -sqr) || (shft > 63-sqr))
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
	size_t knightMoves = LegalJumperMoves(state, moves, KNIGHT, knight_attacks, current_occupation);
	moves += knightMoves;

	Bitboard queenbb = state.bbs[trn * NUMBER_PIECES + QUEEN];
	Bitboard rookbb = state.bbs[trn * NUMBER_PIECES + ROOK];
	Bitboard bishopbb = state.bbs[trn * NUMBER_PIECES + BISHOP];

	/* Bishop */
	size_t bishopMoves = LegalSliderMoves(state, moves, queenbb | bishopbb, bishop_directions, current_occupation, other_occupation );
	moves += bishopMoves;

	/* Rook */
	size_t rookMoves = LegalSliderMoves(state, moves, rookbb | queenbb, rook_directions, current_occupation, other_occupation );
	moves += rookMoves;

	/* King */
	size_t kingMoves = LegalJumperMoves(state, moves, KING, neighbours, current_occupation );
	moves += kingMoves;

	numMoves += kingMoves + rookMoves + bishopMoves + knightMoves;

	return numMoves;
}

void PrettyPrint(const State& state)
{
	// collect information
	std::string pos[8][8];
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
			pos[i][j] = ' ';

	const std::string PIECE_STRINGS = "NBRQKP";
	const std::string piece_strings = "nbrqkp";

	for (int i = 0; i < NUMBER_PIECES; ++i)
	{
		Bitboard wocc = state.bbs[i];
		Bitboard bocc = state.bbs[NUMBER_PIECES+i];
		Piece piece = static_cast<Piece>(i);
		for (int i = 0; i < 64; i++)
		{
			int j = (7 - int(i / 8)) % 8;
			int k = i % 8;
			if (wocc & squares[i])
				pos[j][k] = PIECE_STRINGS[piece];
			if (bocc & squares[i])
				pos[j][k] = piece_strings[piece];
		}
	}
	
	// print out the board
	std::string baseline = "+---";
	for (auto j = 0; j < 7; j++)
		baseline += "+---";
	baseline += "+\n";

	std::string output = baseline;
	for (auto i = 0; i < 8; i++)
	{
		for (auto j = 0; j < 8; j++)
			output += "| " + pos[i][j] + " ";
		output += "|\n";
		output += baseline;
	}

	std::cout << output;
	Bitboard ep = state.bbs[13];

	if (ep)
	{
		std::cout << "en-passant: ";
		for (int i = 0; i < 63; i++)
		{
			if (squares[i] & ep)
				std::cout << SquareName(Square(i));
		}
		std::cout << std::endl;
	}
	std::cout << "fiftycounter: " << state.c50 << std::endl;
	int castlerights = state.castle;
	const std::string crights = "QKqk";
	std::cout << "castlerights: " << castlerights << " ";
	for (char c : crights)
	{
		if (castlerights % 2)
			std::cout << c;
		castlerights /= 2;
	}

	std::cout << std::endl;
	std::cout << "plies: " << state.plies << std::endl;
	std::cout << "colour to move: " << (!state.turn ? "white" : "black") << std::endl;
}

std::uint64_t hash_combine(uint64_t lhs, uint64_t rhs)
{
	lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
	return lhs;
}
