#include "string_transforms.h"

int main()
{
    State state = StateFromFen();
    PrettyPrint(state);
    return 0;
};