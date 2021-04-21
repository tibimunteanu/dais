#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Input
    {
    public:
        static bool s_JoysticksInitialized;

    public:
        static bool InitJoysticks();

    public:
        static bool PlatformInitJoystycks();
        static void PlatformTerminateJoystycks();
    };
}
