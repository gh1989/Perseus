#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <chrono>
#include "move_generation.h"
#include "random.h"
#include "state.h"
#include "string_transforms.h"

int test()
{
	Move moves[256];
	size_t k = 0;
	State start = StateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	State s;
	size_t n;
	Move m;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	for(int i=0;i<64;i++)
	{
		s = start;
#ifdef _DEBUG
		PrettyPrint(s);
#endif	
		while (true) {
			bool legal_move = false;
			n = GenerateMoves(s, moves);
			k += n;
			int mm = i;
			while(!legal_move && ((mm-i) < n ) )
			{
				m = moves[mm%(n-1)];
				auto sprior = s;
				s.Apply(m);
				if (!isCheck(s, !s.turn)) {
					legal_move = true;
				}
				else
				{
					s = sprior;
					mm++;
				}
			}

#ifdef _DEBUG
			PrettyPrint(s);
			std::cout << "Last move:" << AsUci(m) << std::endl;
#endif
			if (!legal_move || s.c50 > 50)
			{
#ifdef _DEBUG
				std::cout << "Completed game" << std::endl;
				//system("PAUSE");
#endif
				break;
			}
		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Node generation: " << k / std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[knps]" << std::endl;
	system("PAUSE");
	return 0;
}
