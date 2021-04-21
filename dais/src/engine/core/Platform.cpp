#include "engine/core/Platform.h"

namespace dais
{
    ////////////////////////////////////// STATIC MEMBERS /////////////////////////////////////////

    std::vector<Window*> Platform::s_Windows = {};
    std::vector<Monitor*> Platform::s_Monitors = {};
    std::vector<Cursor*> Platform::s_Cursors = {};
    Platform::Callbacks Platform::s_Callbacks = {};



    //////////////////////////////////////// STATIC API ///////////////////////////////////////////

    bool Platform::Init()
    {
        return Platform::PlatformInit();
    }

    void Platform::Terminate()
    {
        if (s_Monitors.size() > 0)
        {
            for (size_t i = 0; i < s_Monitors.size(); i++)
            {
                s_Monitors[i]->RestoreOriginalGammaRamp();
                delete s_Monitors[i];
            }
        }

        if (s_Windows.size() > 0)
        {
            for (size_t i = 0; i < s_Windows.size(); i++)
            {
                delete s_Windows[i];
            }
        }

        Platform::PlatformTerminate();
    }

    void Platform::PollEvents()
    {
        Platform::PlatformPollEvents();
    }

    void Platform::WaitEvents()
    {
        Platform::PlatformWaitEvents();
    }

    void Platform::WaitEventsTimeout(double timeout)
    {
        Platform::PlatformWaitEventsTimeout(timeout);
    }


    bool Platform::IsRawMouseMotionSupported()
    {
        return Platform::PlatformIsRawMouseMotionSupported();
    }

    Window* Platform::OpenWindow(WindowConfig config, FramebufferConfig fbConfig, Monitor* monitor)
    {
        Window* window = Window::Create(config, fbConfig, monitor);
        if (window)
        {
            s_Windows.push_back(window);
        }
        return window;
    }


    const std::vector<Window*>& Platform::GetWindows()
    {
        DAIS_TRACE("[Platform] GetWindows");

        return s_Windows;
    }

    Window* Platform::GetPrimaryWindow()
    {
        DAIS_TRACE("[Platform] GetPrimaryWindow");

        return s_Windows.size() > 0
            ? s_Windows[0]
            : nullptr;
    }

    const std::vector<Monitor*>& Platform::GetMonitors()
    {
        DAIS_TRACE("[Platform] GetMonitors");

        return s_Monitors;
    }

    Monitor* Platform::GetPrimaryMonitor()
    {
        DAIS_TRACE("[Platform] GetPrimaryMonitor");

        return s_Monitors.size() > 0
            ? s_Monitors[0]
            : nullptr;
    }

    const char* Platform::GetKeyName(int32_t key, int32_t scancode)
    {
        if (key != DAIS_KEY_UNKNOWN)
        {
            if (key != DAIS_KEY_KP_EQUAL
                && (key < DAIS_KEY_KP_0 || key > DAIS_KEY_KP_ADD)
                && (key < DAIS_KEY_APOSTROPHE || key > DAIS_KEY_WORLD_2))
            {
                return nullptr;
            }

            scancode = PlatformGetKeyScancode(key);
        }

        return PlatformGetScancodeName(scancode);
    }

    int32_t Platform::GetKeyScancode(int32_t key)
    {
        if (key < DAIS_KEY_SPACE
            || key > DAIS_KEY_LAST)
        {
            DAIS_ERROR("Invalid key %i", key);
            return DAIS_RELEASE;
        }

        return PlatformGetKeyScancode(key);
    }

    Cursor* Platform::CreateCursor(const Image* image, int32_t xHot, int32_t yHot)
    {
        Cursor* cursor = Cursor::Create(image, xHot, yHot);
        if (cursor)
        {
            s_Cursors.push_back(cursor);
        }
        return cursor;
    }

    Cursor* Platform::CreateStandardCursor(int32_t shape)
    {
        if (shape != DAIS_ARROW_CURSOR
            && shape != DAIS_IBEAM_CURSOR
            && shape != DAIS_CROSSHAIR_CURSOR
            && shape != DAIS_POINTING_HAND_CURSOR
            && shape != DAIS_RESIZE_EW_CURSOR
            && shape != DAIS_RESIZE_NS_CURSOR
            && shape != DAIS_RESIZE_NWSE_CURSOR
            && shape != DAIS_RESIZE_NESW_CURSOR
            && shape != DAIS_RESIZE_ALL_CURSOR
            && shape != DAIS_NOT_ALLOWED_CURSOR)
        {
            DAIS_ERROR("Invlid standard cursor 0x%08X", shape);
            return nullptr;
        }

        Cursor* cursor = Cursor::Create(shape);
        if (cursor)
        {
            s_Cursors.push_back(cursor);
        }
        return cursor;
    }

    void Platform::SetMonitorConnectedCallback(MonitorCallback callback)
    {
        s_Callbacks.monitorConnected = callback;
    }

    void Platform::SetMonitorDisconnectedCallback(MonitorCallback callback)
    {
        s_Callbacks.monitorDisconnected = callback;
    }
}
