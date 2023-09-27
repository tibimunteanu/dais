#pragma once

#include "base/base.h"
#include "math/math_types.h"
#include "dais_types.h"

API B8 windowCreate(Arena* pArena, CStringLit title, Vec4U32 rect, Window* out_pWindow);