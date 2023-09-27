#pragma once

#include "base/base.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/library.h"
#include "core/monitor.h"
#include "core/window.h"
#include "math/math.h"
#include "dais_types.h"

B32 platformInit(void);
B32 platformRelease(void);

API B8 daisAwake(void);
API B8 daisStart(App* pApp);
API B8 daisShutdown(App* pApp);
