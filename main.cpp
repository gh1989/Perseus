#pragma once
#include "state.h"

int main()
{
	State s("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	PrettyPrint(s);

	auto e2e4 = CreateMove(e2, e4);
	s.Apply(e2e4);
	s.bbs[0] = OffBit(s.bbs[0], e2);
	PrettyPrint(s);

	system("PAUSE");
	return 0;
}