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

    Cursor* Cursor::Create(int32_t shape)
    {
        int32_t id = 0;

        switch (shape)
        {
            case DAIS_ARROW_CURSOR: id = OCR_NORMAL; break;
            case DAIS_IBEAM_CURSOR: id = OCR_IBEAM; break;
            case DAIS_CROSSHAIR_CURSOR: id = OCR_CROSS; break;
            case DAIS_POINTING_HAND_CURSOR: id = OCR_HAND; break;
            case DAIS_RESIZE_EW_CURSOR: id = OCR_SIZEWE; break;
            case DAIS_RESIZE_NS_CURSOR: id = OCR_SIZENS; break;
            case DAIS_RESIZE_NWSE_CURSOR: id = OCR_SIZENWSE; break;
            case DAIS_RESIZE_NESW_CURSOR: id = OCR_SIZENESW; break;
            case DAIS_RESIZE_ALL_CURSOR: id = OCR_SIZEALL; break;
            case DAIS_NOT_ALLOWED_CURSOR: id = OCR_NO; break;

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
