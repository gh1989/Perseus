#include "string_transforms.h"
#include "network.h"

int main()
{
    State state = StateFromFen();
    ChessNetworkImpl impl;
    impl.inputFromState(state);
    impl.forward(state);
    PrettyPrint(state);
    return 0;
};