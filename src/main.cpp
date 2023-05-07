#include <iostream>
#include <iomanip>
#include <numeric>
#include <torch/torch.h>
#include <torch/nn/functional.h>

#include "network.h"
#include "string_transforms.h"

int main()
{
    ChessInputLayerImpl network;
    State state = StateFromFen();
    std::cout << network.forward(state);
    SimpleChessNet net;
    auto input = network.forward(state);
    auto netOutput = net.forward( input );
    auto value = std::get<0>(netOutput);
    auto policy = std::get<1>(netOutput);
    //std::cout << value.item<float>() << std::endl;
    std::cout << policy.squeeze() << std::endl;
    return 0;
}