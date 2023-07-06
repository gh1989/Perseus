#ifndef NETWORK_H
#define NETWORK_H

#include <pybind11/pybind11.h>
#include <torch/torch.h>
#include <torch/nn/functional.h>

#include "state.h"

class ChessNetworkImpl : public torch::nn::Module
{
private:
  // Input layer: 18 x 8 x 8
  torch::nn::Conv2d input_conv{nullptr};
  torch::nn::Functional input_relu{nullptr};

  // Hidden layer: output a scalar value
  torch::nn::Conv2d value_conv{nullptr};
  torch::nn::Functional value_relu{nullptr};
  torch::nn::Linear value_fc{nullptr};
  torch::nn::Functional value_tanh{nullptr};

  // Policy head: output a probability distribution over the legal moves
  torch::nn::Conv2d policy_conv{nullptr};
  torch::nn::Functional policy_relu{nullptr};
  torch::nn::Linear policy_fc{nullptr};
  torch::nn::Functional policy_softmax{nullptr};

public:
    ChessNetworkImpl() 
    {
        input_conv = register_module("input_conv", torch::nn::Conv2d(torch::nn::Conv2dOptions(18, 32, 3).stride(1).padding(1)));
        input_relu = torch::nn::Functional(torch::relu);

        value_conv = register_module("value_conv", torch::nn::Conv2d(torch::nn::Conv2dOptions(32, 1, 1)));
        value_relu = torch::nn::Functional(torch::relu);
        value_fc = register_module("value_fc", torch::nn::Linear(8*8, 1));
        value_tanh = torch::nn::Functional(torch::tanh);

        policy_conv = register_module("policy_conv", torch::nn::Conv2d(torch::nn::Conv2dOptions(32, 8, 1)));
        policy_relu = torch::nn::Functional(torch::relu);
        policy_fc = register_module("policy_fc", torch::nn::Linear(8*8*8, 64));
        policy_softmax = torch::nn::Functional(torch::nn::functional::softmax, 1);
    }

    auto inputFromState(const State& board)
    {
        // Define the shape of the tensor
        const int nRows = 8;
        const int nCols = 8;
        const int nChannels = 18;

        // Create a tensor of zeros with the appropriate shape
        auto inputTensor = torch::zeros({nChannels, nRows, nCols});

        // Encode the piece positions
        for(int i=0; i<13; i++)
            for(auto j : BitboardRange(board.getBitboard(i)))
            {
                // File
                int m = j % 8;
                // Rank
                int n = j / 8;
                inputTensor[i][n][m] = 1;
            }

        // Encode the current player
        if (!board.isBlackMove())
            inputTensor[13].fill_(1);

        // Encode the castling rights
        if (board.getCastleRights() & Castling::BK)
            inputTensor[14][0].fill_(1);
        if (board.getCastleRights() & Castling::WK)
            inputTensor[15][0].fill_(1);
        if (board.getCastleRights() & Castling::BK)
            inputTensor[16][7].fill_(1);
        if (board.getCastleRights() & Castling::BQ)
            inputTensor[17][7].fill_(1);

        return inputTensor;
    }

    auto forward(const State& board)
    {
        auto inputTensor = inputFromState( board );
        auto x = input_conv->forward(inputTensor);
        x = input_relu->forward(x);

        // Value head
        auto v = value_conv->forward(x);
        v = value_relu->forward(v);
        v = v.view({-1, 8*8});
        v = value_fc->forward(v);
        v = value_tanh->forward(v);

        // Policy head
        auto p = policy_conv->forward(x);
        p = policy_relu->forward(p);
        p = p.view({-1, 8*8*8});
        p = policy_fc->forward(p);
        p = policy_softmax->forward(p);

        return std::make_tuple(v, p);
    }
};

namespace py = pybind11;

PYBIND11_MODULE(chess_network, m)
{
    py::class_<ChessNetworkImpl>(m, "ChessNetwork")
        .def(py::init<>())
        .def("forward", &ChessNetworkImpl::forward);
}

#endif