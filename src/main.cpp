
#include "string_transforms.h"
#include "move_generation.h"
#include <iostream>
int main()
{
	State start = StateFromFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	PrettyPrint(start);
	std::cout << "Hello, world: here's move generation that doesn't work at all" << std::endl;
	Move moves[256];
	GenerateMoves(start, moves);
	for(int i=0; i<20; i++)
		std::cout << AsUci(moves[i]) << std::endl;
	
	std::cout << start.bbs[12] << std::endl;
	return 0;
}