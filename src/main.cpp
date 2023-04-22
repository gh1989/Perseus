#include <iostream>
#include <iomanip>
#include <numeric>

#include "network.h"
#include "string_transforms.h"

int main()
{
    ChessInputLayerImpl network;
    State state = StateFromFen();
    std::cout << network.forward(state);
    return 0;
}