
#include <torch/torch.h>
#include <state.h>

class ChessInputLayerImpl : public torch::nn::Module
{
public:
    ChessInputLayerImpl() {}

    torch::Tensor forward(const State& board)
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

};

TORCH_MODULE(ChessInputLayer);