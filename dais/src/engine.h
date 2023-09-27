#pragma once

#include "base/base.h"
#include "core/log.h"
#include "core/arena.h"
#include "math/math.h"
#include "platform/platform.h"
#include "platform/memory.h"
#include "platform/library.h"
#include "platform/monitor.h"
#include "platform/window.h"
#include "engine_types.h"

API B8 engineAwake(void);
API B8 engineStart(App* pApp);
API B8 engineShutdown(App* pApp);
