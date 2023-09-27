#pragma once

#include "engine.h"

extern B8 appRegister(App* pApp);

int main(void) {
    if (!engineAwake()) {
        return -1;
    }

    App app = {0};

    if (!appRegister(&app)) {
        return -2;
    }

    if (!app.awake || !app.start || !app.update || !app.render || !app.shutdown) {
        logFatal("App is missing required function pointers");
        return -3;
    }

    if (!engineStart(&app)) {
        logFatal("Failed to start engine");
        return -4;
    }

    if (!engineShutdown(&app)) {
        logError("Engine did not shutdown gracefully");
        return -5;
    }

    return 0;
}
