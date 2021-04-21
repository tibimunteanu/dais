#pragma once

#include "engine/core/Base.h"

namespace dais
{
    class Joystick
    {
    public:

    public:
        void OnJoystickConnected();
        void OnJoystickDisconnected();
        void OnJoystickAxis(int32_t axis, float value);
        void OnJoystickButton(int32_t button, int8_t value);
        void OnJoystickHat(int32_t hat, int8_t value);
    };
}
