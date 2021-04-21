#pragma once

#include "platform/windows/WindowsBase.h"
#include "engine/core/Cursor.h"

namespace dais
{
    class WindowsCursor : public Cursor
    {
    public:
        HCURSOR m_Handle = nullptr;

    public:
        WindowsCursor();
        virtual ~WindowsCursor();
    };
}
