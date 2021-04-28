
#ifndef TIDE_ENGINE_H
#define TIDE_ENGINE_H

#include "window.h"
#include <memory>

extern Unique<Engine> engine;

#ifdef TIDE_ENGINE_IMPLEMENTATION
Unique<Engine> engine;
#endif // TIDE_MAKE_ENGINE


#endif // TIDE_ENGINE_H