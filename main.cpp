#pragma once
#include "state.h"
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

size_t testPointerPassing2(Move *moves)
{
	*moves++ = CreateMove(d2, d4);
	return 1;
}

size_t testPointerPassing3(Move *moves)
{
	*moves++ = CreateMove(f2, f4);
	return 1;
}


size_t testPointerPassing(Move *moves)
{
	size_t numMoves = 0;
	*moves++ = CreateMove(e2,e4);
	numMoves++;
	size_t numMoves2 = testPointerPassing2(moves);
	moves += numMoves2;
	size_t numMoves3 = testPointerPassing3(moves);
	moves += numMoves3;
	return numMoves + numMoves2 + numMoves3;
}

int main()
{
	State s("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	
	Move moves[256];
	
	size_t numMoves = 0;
	numMoves += testPointerPassing(moves);
	for (int i = 0; i < numMoves; ++i)
	{
		Move m = moves[i];
		auto str = AsUci(m);
		std::cout << str << std::endl;
	}
	
	
	s = State("8/8/3p4/4B3/8/8/8/8 b - - 0 5"); // d6d5, d6e5
	s = State("8/3p4/8/4B3/8/8/8/8 b - - 0 5"); // d7d5, d7d6
	s = State("8/8/8/4B3/8/8/3p4/8 b - - 0 5"); // d2d1N, d2d1R, d2d1Q, d2d1B
	s = State("8/8/4p3/4B3/8/8/3P4/8 w - - 0 5"); // d2d4, d2d3
	s = State("8/8/4p3/4B3/8/4b3/3P4/8 w - - 0 5"); // d2d3, d2d4, d2e3
	s = State("8/3P4/4p3/4B3/8/4b3/8/8 w - - 0 5"); // d7d8N,  d7d8R,  d7d8Q,  d7d8B
	s = State("8/8/4p3/4B3/8/8/P7/7b w - - 0 5"); // a2a3, a2a4
	s = State("8/8/4p3/4B3/8/8/7P/7b w - - 0 5"); // h2h3, h2h4
	s = State("8/8/4p3/4B3/8/6b1/7P/8 w - - 0 5");
	s = State("8/8/8/5N2/8/6n1/8/8 w - - 0 5");
	s = State("3k4/8/3Q4/8/8/6n1/8/8 w - - 0 5");
	s = State("3k4/8/8/8/8/8/3PN3/3K4 w - - 0 5");
	s = State("3k4/8/8/8/8/8/3BN3/3K4 w - - 0 5");
	s = State("3k4/7P/8/2Q5/7R/5N2/P2B4/3K4 w - - 0 5");
	s = State("3k4/8/8/1p1p4/8/2N5/p1Rnb3/Qn1K4 w - - 0 5");
	s = State("8/8/8/8/8/8/pp6/Qn6 w - - 0 5");
	//s = State("8/8/8/8/8/8/PP6/qN6 b - - 0 5");
	auto s0 = s;
	PrettyPrint(s);
	auto n = GenerateMoves(s, moves);
	for (int i = 0; i < n; ++i)
	{
		Move m = moves[i];
		auto str = AsUci(m);
		std::cout << str << std::endl;
		s.Apply(m);
		PrettyPrint(s);
		s = s0;
	};


	n = 1000000;
	size_t k = 0;
	s = State("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	for (int i = 0; i < n; i++)
	{
		auto n = GenerateMoves(s, moves);
		k += n;
		if (n > 0)
		{
			//auto rand_i = n > 1 ? rand() % (n - 1) : 0;
			//std::cout << "Randomly selected move " << rand_i << " of " << n << std::endl;
			auto m = moves[0];
			s.Apply(m);
			isCheck(s);
			try
			{
				//s.Apply(m);
				//auto str = AsUci(m);
				//std::cout << "M:" << rand_i << ":" << str << std::endl;
				//PrettyPrint(s);
				//if(str == "g7g6")
				//	std::cout << "PAUSE";
			}
			catch (...)
			{

			}
			//PrettyPrint(s);
		}
		else
		{
			// We will never reach this point currently, because checkmate and checks are not
			// currently encoded. The game reaches a state where the first move is a knight or king
			// move and will probably move back and forth.
			std::cout << "Completed game" << std::endl;
			PrettyPrint(s);
			s = State("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	PrettyPrint(s);
	std::cout << "Speed: " << k/std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[knps]" << std::endl;

	system("PAUSE");

	return 0;
}