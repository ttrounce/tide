#ifndef TIDE_VAO_H
#define TIDE_VAO_H

#include <glad/glad.h>
#include <vector>

struct VAO
{
    GLuint handle;
    std::vector<GLuint> buffers;

    ~VAO();
};

void SetBuffer(VAO& vao, int index, int size, const std::vector<float>& data);

#endif // TIDE_VAO_H