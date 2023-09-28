#pragma once

#include <dais.h>

API GameConfig configure(void);
API Result awake(void);
API Result start(void);
API Result update(void);
API Result render(void);
API Result shutdown(void);
