#pragma once

#include "base/base.h"
#include "core/arena.h"
#include "math/math_types.h"
#include "platform/platform_types.h"

typedef enum GameStage {
    GAME_STAGE_NONE = 0,
    GAME_STAGE_AWAKING = 1,
    GAME_STAGE_AWAKEN = 2,
    GAME_STAGE_STARTING = 3,
    GAME_STAGE_STARTED = 4,
    GAME_STAGE_RUNNING = 5,
    GAME_STAGE_SHUTTING_DOWN = 6
} GameStage;

typedef struct GameConfig {
    char name[256];
    Vec4U32 startRect;
} GameConfig;

typedef struct Game {
    GameStage stage;
    GameConfig config;

    GameConfig (*configure)(void);
    Result (*awake)(void);
    Result (*start)(void);
    Result (*update)(void);
    Result (*render)(void);
    Result (*shutdown)(void);
} Game;

typedef struct Engine {
    Game* pGame;
    B8 isRunning;
    Platform* pPlatform;
} Engine;

global Engine* pDais;
