#include "vao.h"

VAO::~VAO()
{
    glDeleteVertexArrays(1, &handle);
    glDeleteBuffers(buffers.size(), &buffers[0]);
}

void SetBuffer(VAO& vao, int index, int size, const std::vector<float>& data)
{
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(index);
    vao.buffers.push_back(buffer);
}