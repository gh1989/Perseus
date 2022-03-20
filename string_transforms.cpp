#include <sstream>
#include <iostream>
#include "move.h"
#include "state.h"
#include "string_transforms.h"
#include "types.h"

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
		Bitboard bocc = state.bbs[NUMBER_PIECES + i];
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

/* UCI and string operations */
std::string AsUci(Move move) {
	std::stringstream ss;
	ss << SquareName(GetFrom(move));
	ss << SquareName(GetTo(move));
	if (SpecialMoveType(move) == PROMOTE)
		ss << PieceStringLower(PromotionPiece(move));
	return  ss.str();
}

State StateFromFen(std::string fen) 
{
	const std::string castle_str = "QKqk";
	State state;
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
		throw std::runtime_error("Bad FEN");

	auto posstring = strings[0];
	auto colour_string = strings[1];
	auto castlestring = strings[2];
	auto epstring = strings[3];
	auto fiftycounter = strings[4];
	auto moveclock = strings[5];

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
				uint8_t pid = this_piece_colour * NUMBER_PIECES + piece;
				state.bbs[pid] = OnBit(state.bbs[pid], square);
				sub_ite++;
			}
		}
		if (sub_ite != 8)
			throw std::runtime_error("Bad FEN");

		ite -= 1;
	}

	if (ite != 0)
		throw std::runtime_error("Bad FEN");

	if (castlestring != "-") {
		uint8_t castlesum = 0;
		for (char c : castlestring) {
			found = castle_str.find(c);
			if (found != std::string::npos)
				castlesum += 1 << found;
		}
		state.castle = castlesum;
	}
	else
		state.castle = 0;

	// 0 for w, 1 for b.
	state.turn = (colour_string != "w");

	if (epstring != "-")
		state.bbs[13] = BitboardFromString(epstring);
	else
		state.bbs[13] = 0;

	if (fiftycounter != "-") {
		uint8_t counter = std::stoi(fiftycounter);
		if (counter >= 999)
			throw std::runtime_error("Bad FEN");
		state.c50 = counter;
	}
	else
		state.c50 = 0;

	if (moveclock != "-") {
		unsigned short counter = (std::stoi(moveclock) - 1);
		if (counter > 999)
			throw std::runtime_error("Bad FEN");
		state.plies = 2 * counter + state.turn;
	}

	return state;
}
