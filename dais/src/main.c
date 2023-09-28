#include <engine.h>

int main(void) {
    // TODO: editor mode or standalone mode
    if (!engineRun()) {
        logError("Engine did not shutdown gracefully");
        return -1;
    }

    return 0;
}
