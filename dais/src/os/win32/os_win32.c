#include "os/win32/os_win32.h"

#include "core/log.h"

#ifdef OS_WINDOWS

#    pragma comment(lib, "user32")
#    define WINDOW_OS_STATE  Win32Window win32;
#    define MONITOR_OS_STATE Win32Monitor win32;

U64 osMemoryPageSizeGet(void) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

void* osMemoryReserve(U64 size) {
    size = roundUpToMultipleOf(size, gigabytes(1));
    void* pResult = VirtualAlloc(0, size, MEM_RESERVE, PAGE_NOACCESS);
    return pResult;
}

void osMemoryRelease(void* pMemory, U64 size) {
    VirtualFree(pMemory, 0, MEM_RELEASE);
}

void osMemoryCommit(void* pMemory, U64 size) {
    size = roundUpToMultipleOf(size, osMemoryPageSizeGet());
    VirtualAlloc(pMemory, size, MEM_COMMIT, PAGE_READWRITE);
}

void osMemoryDecommit(void* pMemory, U64 size) {
    VirtualFree(pMemory, size, MEM_DECOMMIT);
}

void osMemoryProtect(void* pMemory, U64 size, MemoryAccessFlags flags) {
    size = roundUpToMultipleOf(size, osMemoryPageSizeGet());

    DWORD newFlags = 0;
    switch (flags) {
        case MEMORY_ACCESS_FLAGS_READ: newFlags = PAGE_READONLY; break;
        case MEMORY_ACCESS_FLAGS_READ | MEMORY_ACCESS_FLAGS_WRITE: newFlags = PAGE_READWRITE; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE: newFlags = PAGE_EXECUTE; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE | MEMORY_ACCESS_FLAGS_READ: newFlags = PAGE_EXECUTE_READ; break;
        case MEMORY_ACCESS_FLAGS_EXECUTE | MEMORY_ACCESS_FLAGS_READ | MEMORY_ACCESS_FLAGS_WRITE:
            newFlags = PAGE_EXECUTE_READWRITE;
            break;
        case MEMORY_ACCESS_FLAGS_EXECUTE | MEMORY_ACCESS_FLAGS_WRITE: newFlags = PAGE_EXECUTE_WRITECOPY; break;
        default: newFlags = PAGE_NOACCESS; break;
    }

    DWORD oldFlags = 0;
    VirtualProtect(pMemory, size, newFlags, &oldFlags);
}

void* osLibraryOpen(const char* path) {
    HMODULE module = LoadLibraryA(path);
    return (void*)module;
}

void osLibraryClose(void* pHandle) {
    HMODULE module = (HMODULE)pHandle;
    FreeLibrary(module);
}

void* osLibraryLoadFunction(void* pHandle, const char* name) {
    HMODULE module = (HMODULE)pHandle;
    void* pfnResult = GetProcAddress(module, name);
    return pfnResult;
}

B8 osWindowCreate(Arena* arena, const char* title, Vec4U32 rect, Window* out_pWindow) {
    memoryZero(out_pWindow, sizeof(Window));

    HMODULE module = GetModuleHandle(0);
    HICON icon = LoadIcon(module, IDI_APPLICATION);
    WNDCLASSA windowClass = {
        .style = CS_DBLCLKS,
        .lpfnWndProc = DefWindowProc,
        .hInstance = module,
        .hIcon = icon,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = "DaisWindowClass",
    };

    if (!RegisterClassA(&windowClass)) {
        logError("Failed to register window class");
        return false;
    }

    Vec4U32 windowRect = rect;

    DWORD windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;
    DWORD windowStyleEx = WS_EX_APPWINDOW;

    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, FALSE, windowStyleEx);

    windowRect.x += borderRect.left;
    windowRect.y += borderRect.top;

    windowRect.z += borderRect.right - borderRect.left;
    windowRect.w += borderRect.bottom - borderRect.top;

    HWND windowHandle = CreateWindowExA(
        windowStyleEx,
        windowClass.lpszClassName,
        title,
        windowStyle,
        windowRect.x,
        windowRect.y,
        windowRect.z,
        windowRect.w,
        0,
        0,
        module,
        0
    );

    if (!windowHandle) {
        logError("Failed to create window");
        return false;
    }

    Win32Window* win32Window = arenaPushZero(arena, sizeof(Win32Window));
    win32Window->instance = module;
    win32Window->handle = windowHandle;

    out_pWindow->pOsState = win32Window;

    ShowWindow(windowHandle, SW_SHOW);

    return true;
}

#endif
