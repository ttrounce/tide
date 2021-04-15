#include "vao.h"

namespace tide
{

void SetBuffer(VAO& vao, int index, int size, float* data, int len)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, len * sizeof(float), data, GL_STATIC_DRAW);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    vao.buffers.push_back(buffer);
}

}