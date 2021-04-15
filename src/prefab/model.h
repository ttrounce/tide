#ifndef TIDE_MODEL_PREFAB_H
#define TIDE_MODEL_PREFAB_H

#include <memory>
#include "engine/gfx/vao.h"

namespace tide
{

extern std::shared_ptr<tide::VAO> modelQuad;

void InitialisePrefab();

}

#endif // TIDE_MODEL_PREFAB_H