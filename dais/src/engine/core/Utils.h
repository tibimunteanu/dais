#pragma once

#include "Base.h"

namespace dais
{
    class Utils
    {
    public:
        static float fminf(float a, float b);
        static float fmaxf(float a, float b);

        template<typename T>
        static int32_t indexOf(const std::vector<T>& vec, const T& element)
        {
            for (int32_t i = 0; i < vec.size(); i++)
            {
                if (vec[i] == element)
                {
                    return i;
                }
            }
            return -1;
        }
    };
}
