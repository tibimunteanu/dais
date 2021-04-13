#include "engine/core/Utils.h"

namespace dais
{
    float Utils::fminf(float a, float b)
    {
        if (a != a) return b;
        if (b != b) return a;
        if (a < b) return a;
        return b;
    }

    float Utils::fmaxf(float a, float b)
    {
        if (a != a) return b;
        if (b != b) return a;
        if (a > b) return a;
        return b;
    }
}

