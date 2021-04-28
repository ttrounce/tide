#include "model.h"

Unique<VAO> modelQuad;

void InitialisePrefab()
{
    auto vertsQuad = std::vector<float>{
        0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0
    };
    auto texesQuad = std::vector<float>{
        0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0
    };

    modelQuad = std::make_unique<VAO>();
    glGenVertexArrays(1, &modelQuad->handle);
    glBindVertexArray(modelQuad->handle);
    SetBuffer(*modelQuad.get(), 0, 2, vertsQuad);
    SetBuffer(*modelQuad.get(), 1, 2, texesQuad);
}
