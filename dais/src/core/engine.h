#pragma once

#include "dais_types.h"
#include "core/arena.h"

typedef struct Engine {
    App* pApp;
    B8 isRunning;
    Arena* pArena;
} Engine;

extern Engine engine;

API B8 engineAwake(void);
API B8 engineStart(App* pApp);
API B8 engineShutdown(App* pApp);
