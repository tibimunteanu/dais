#include "core/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

typedef struct LogState {
    LogConfig config;
} LogState;

//
prv LogState* pState;

//
pub Result logInit(Arena* pArena, LogConfig config) {
    pState = arenaPushStructZero(pArena, LogState);
    pState->config = config;

    return OK;
}

pub void logRelease(void) {
    pState = NULL;
}

pub void logOutput(LogLevel level, CStringLit message, ...) {
    if (pState == NULL) {
        return;
    }

    static CString levelStrings[] = {"FATAL :: ", "ERROR :: ", " WARN :: ", " INFO :: ", "DEBUG :: ", "TRACE :: "};

    __builtin_va_list args;
    va_start(args, message);
    char buffer[kilobytes(32)];
    I32 bytesWritten = vsnprintf(buffer, kilobytes(32), message, args);
    buffer[bytesWritten] = 0;
    printf("%s%s\n", levelStrings[level], buffer);
}
