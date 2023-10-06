#pragma once

#include "base/base.h"
#include "core/arena.h"
#include "math/math_types.h"
#include "platform/platform_types.h"

API fn windowCreate(Arena* pArena, CStringLit title, Vec4U32 rect, Window* out_pWindow);
