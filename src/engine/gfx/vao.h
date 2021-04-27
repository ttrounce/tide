#ifndef TIDE_VAO_H
#define TIDE_VAO_H

#include <glad/glad.h>
#include <vector>

struct VAO
{
    GLuint handle;
    std::vector<GLuint> buffers;

    ~VAO()
    {
        glDeleteVertexArrays(1, &handle);
        glDeleteBuffers(buffers.size(), &buffers[0]);
    }
};

void SetBuffer(VAO& vao, int index, int size, float* data, int len);

#endif // TIDE_VAO_H