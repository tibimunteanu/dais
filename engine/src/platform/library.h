#pragma once

#include "base/base.h"

API void* libraryOpen(const CString path);
API void libraryClose(void* pHandle);
API void* libraryLoadFunction(void* pHandle, const CString name);
