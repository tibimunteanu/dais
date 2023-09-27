#pragma once

#include "base/base.h"

API void* libraryOpen(CStringLit path);
API void libraryClose(void* pHandle);
API void* libraryLoadFunction(void* pHandle, CStringLit name);