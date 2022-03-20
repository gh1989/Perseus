#pragma once
#include <functional>
#include <iostream>
#include <string>
#include <chrono>
#include "move_generation.h"
#include "random.h"
#include "state.h"
#include "string_transforms.h"


int main()
{
	Move moves[256];
	size_t k = 0;
	State start = StateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	State s;
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	unsigned long r;
	for(int i=0;i<1;i++)
	{
		s = start;
		while (true) {
			bool legal_move = false;
			auto n = GenerateMoves(s, moves);
			k += n;
			while(!legal_move)
			{
				r = xorshf96();
				auto m = moves[r%(n-1)];
				auto sprior = s;
				s.Apply(m);
				if (!isCheck(s, !s.turn)) {
					legal_move = true;
				}
				else
					s = sprior;
			}
#ifdef _DEBUG
			PrettyPrint(s);
#endif
			if (!legal_move || s.c50 > 50)
			{
#ifdef _DEBUG
				std::cout << "Completed game" << std::endl;
#endif
				break;
			}
		}
	}
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Speed: " << k / std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[knps]" << std::endl;
	system("PAUSE");
	//PrettyPrint(s);
	return 0;
}
