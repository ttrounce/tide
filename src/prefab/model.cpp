#include "model.h"

namespace tide
{

std::shared_ptr<tide::VAO> modelQuad;

void InitialisePrefab()
{
    float vertsQuad[] = {
        0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0
    };
    float texesQuad[] = {
        0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0
    };

    modelQuad = std::make_shared<tide::VAO>();
    glGenVertexArrays(1, &modelQuad->handle);
    glBindVertexArray(modelQuad->handle);
    SetBuffer(*modelQuad.get(), 0, 2, vertsQuad, 12);
    SetBuffer(*modelQuad.get(), 1, 2, texesQuad, 12);
}

}
