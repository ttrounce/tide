#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fmt/core.h>

namespace tide
{

void SetMinMag(GLenum target, GLint min, GLint mag)
{
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag);
}

void LoadTexture(GLuint handle, void* data, GLsizei width, GLsizei height, GLint internalFormat, GLenum format)
{
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
}

void LoadTextureArray(GLuint handle, void* data, GLsizei width, GLsizei height, GLsizei depth, GLint internalFormat, GLenum format)
{
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, internalFormat, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data);
}

/**
 * @param internalFormat see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexStorage3D.xhtml for valid formats.
 */
void SetTextureArrayStorage(GLuint handle, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, internalFormat, width, height, depth);
}

void LoadTextureArrayLayer(GLuint handle, void* data, GLint x, GLint y, GLsizei width, GLsizei height, GLint layer, GLenum format)
{
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, layer, width, height, 1, format, GL_UNSIGNED_BYTE, data);
}

bool CreateQuickTexture(GLuint target, GLuint* textureHandle, std::string path, GLint min, GLint mag)
{
    GLint w, h, c;
    uchar* data = stbi_load(path.c_str(), &w, &h, &c, STBI_rgb_alpha);

    if(!data)
    {
        fmt::print("[TIDE] Unable to load texture due to an unknown error\n");
        return false;
    }

    GLenum format = 0;

    switch(c)
    {
        case STBI_rgb:
            format = GL_RGB;
            break;
        case STBI_rgb_alpha:
            format = GL_RGBA;
            break;
        default:
            fmt::print("[TIDE] Unable to load texture due to an unexpected pixel format\n");
            stbi_image_free(data);
            return false;
    }

    glGenTextures(1, textureHandle);
    glBindTexture(target, *textureHandle);
    SetMinMag(target, min, mag);
    LoadTexture(*textureHandle, (void*) data, w, h, format, format);
    glBindTexture(target, 0);
    stbi_image_free(data);
    return true;
}

}