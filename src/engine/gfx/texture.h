#include "../types.h"

#include <glad/glad.h>
#include <string>

namespace tide
{

void SetMinMag(GLenum target, GLint min, GLint mag);
void LoadTexture(GLuint handle, void* data, GLsizei width, GLsizei height, GLint internalFormat, GLenum format);
void LoadTextureArray(GLuint handle, void* data, GLsizei width, GLsizei height, GLsizei depth, GLint internalFormat, GLenum format);
void SetTextureArrayStorage(GLuint handle, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
void LoadTextureArrayLayer(GLuint handle, void* data, GLint x, GLint y, GLsizei width, GLsizei height, GLint layer, GLenum format);
bool CreateQuickTexture(GLuint target, GLuint* textureHandle, std::string path, GLint min, GLint mag);

}