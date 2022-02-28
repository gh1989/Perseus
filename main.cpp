#pragma once
#include "state.h"
#include <functional>
#include <iostream>

int main()
{
	State s("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	
	Move moves[256];

	//PrettyPrint(s);
	std::cout << hash_combine(s.bbs[0], s.bbs[1]) << std::endl;
	s.Apply(CreateMove(e2, e4));
	std::cout << hash_combine(s.bbs[0], s.bbs[1]) << std::endl;
	s.Apply(CreateMove(g8, f6));
	s.Apply(CreateMove(e4, e5));
	s.Apply(CreateMove(h7, h6));
	s.Apply(CreateMove(e5, f6));
	s.Apply(CreateMove(h6, h5));
	s.Apply(CreateMove(f6, g7));
	s.Apply(CreateMove(h5, h4));
	s.Apply(CreatePromotion( g7, f8, ROOK ));

	PrettyPrint(s);
	GenerateMoves(s, moves);
	for (auto m : moves)
	{
		std::string s = AsUci(m);
	}

	system("PAUSE");
	return 0;
}