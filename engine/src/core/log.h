#pragma once

#include "base/base.h"
#include "core/arena.h"

#if defined(_DEBUG)
    #define LOG_WARN_ENABLED  1
    #define LOG_INFO_ENABLED  1
    #define LOG_DEBUG_ENABLED 1
    #define LOG_TRACE_ENABLED 1
#endif


typedef enum LogLevel {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} LogLevel;

typedef struct LogConfig {
    char fileName[1024];
} LogConfig;


API Result logInit(Arena* pArena, LogConfig config);

API void logRelease(void);

API void logOutput(LogLevel level, CStringLit message, ...);


#define logFatal(message, ...) logOutput(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#define logError(message, ...) logOutput(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#if LOG_WARN_ENABLED
    #define logWarning(message, ...) logOutput(LOG_LEVEL_WARNING, message, ##__VA_ARGS__);
#else
    #define logWarning(message, ...)
#endif

#if LOG_INFO_ENABLED
    #define logInfo(message, ...) logOutput(LOG_LEVEL_INFO, message, ##__VA_ARGS__);
#else
    #define logInfo(message, ...)
#endif

#if LOG_DEBUG_ENABLED
    #define logDebug(message, ...) logOutput(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);
#else
    #define logDebug(message, ...)
#endif

#if LOG_TRACE_ENABLED
    #define logTrace(message, ...) logOutput(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);
#else
    #define logTrace(message, ...)
#endif
