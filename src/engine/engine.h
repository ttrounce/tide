
#ifndef TIDE_ENGINE_H
#define TIDE_ENGINE_H

#include "window.h"
#include <memory>

extern std::unique_ptr<tide::ENGINE> engine;

#ifdef TIDE_ENGINE_IMPLEMENTATION
std::unique_ptr<tide::ENGINE> engine;
#endif // TIDE_MAKE_ENGINE

#endif // TIDE_ENGINE_H