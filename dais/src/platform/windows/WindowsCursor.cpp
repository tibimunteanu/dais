#include "platform/windows/WindowsPlatform.h"

namespace dais
{
    Cursor* Cursor::Create(const Image* image, int32_t xHot, int32_t yHot)
    {
        HCURSOR cursorHandle = (HCURSOR)WindowsWindow::CreateIcon(image, xHot, yHot, false);
        if (!cursorHandle)
        {
            DAIS_ERROR("Failed to create cursor!");
            return nullptr;
        }

        WindowsCursor* cursor = new WindowsCursor();
        cursor->m_Handle = cursorHandle;

        return cursor;
    }

    Cursor* Cursor::Create(CursorShape shape)
    {
        int32_t id = 0;

        switch (shape)
        {
            case CursorShape::Arrow: id = OCR_NORMAL; break;
            case CursorShape::IBeam: id = OCR_IBEAM; break;
            case CursorShape::Crosshair: id = OCR_CROSS; break;
            case CursorShape::PointingHand: id = OCR_HAND; break;
            case CursorShape::ResizeEW: id = OCR_SIZEWE; break;
            case CursorShape::ResizeNS: id = OCR_SIZENS; break;
            case CursorShape::ResizeNWSE: id = OCR_SIZENWSE; break;
            case CursorShape::ResizeNESW: id = OCR_SIZENESW; break;
            case CursorShape::ResizeAll: id = OCR_SIZEALL; break;
            case CursorShape::NotAllowed: id = OCR_NO; break;

            default: 
            {
                DAIS_ERROR("Unknown standard cursor!");
                return nullptr;
            }
        }

        HCURSOR cursorHandle = (HCURSOR)LoadImageW(NULL, MAKEINTRESOURCEW(id), 
            IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

        if (!cursorHandle)
        {
            DAIS_ERROR("Failed to create standard cursor!");
            return nullptr;
        }

        WindowsCursor* cursor = new WindowsCursor();
        cursor->m_Handle = cursorHandle;

        return cursor;
    }


    WindowsCursor::WindowsCursor()
    {
    }

    WindowsCursor::~WindowsCursor()
    {
        if (m_Handle)
        {
            DestroyIcon((HICON)m_Handle);
        }
    }
}
