
#include "../engine.h"
#include "../io.h"
#include "font.h"

#include <glm/ext/matrix_common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glad/glad.h>
#include FT_FREETYPE_H
#include <ft2build.h>
#include <fmt/core.h>
#include <array>

FT_Library library;
Unique<FontRenderer> fontRenderer;

void InitFreeType()
{
    int error = FT_Init_FreeType(&library);
    if (error)
    {
        fmt::print("[TIDE] Error initialising FreeType\n");
    }
    else
    {
        fmt::print("[TIDE] Successfully initialised FreeType\n");
    }
    fontRenderer = std::make_unique<FontRenderer>();
}

void FreeFreeType()
{
    FT_Done_FreeType(library);
}

Font::~Font()
{
    FT_Done_Face(ftFace);
    for(auto e : glyphs)
    {
        free(e.second->bitmap);
        delete e.second;
    }
    glyphs.clear();
    glDeleteTextures(1, &textureHandleGL);
}

FontRenderer::FontRenderer()
{
    // Generate VAO model
    auto vertices = std::vector<float>{ 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
    auto texels = std::vector<float>{ 0.0, 0.0, 0.99, 0.99, 0.99, 0.0, 0.0, 0.0, 0.0, 0.99, 0.99, 0.99 };

    glGenVertexArrays(1, &vao.handle);
    glBindVertexArray(vao.handle);
    SetBuffer(vao, 0, 2, vertices);
    SetBuffer(vao, 1, 2, texels);
    glBindVertexArray(0);

    // Create Shader Program
    std::string vert = ReadFile("shaders/font_vert.glsl");
    std::string frag = ReadFile("shaders/font_frag.glsl");

    program = glCreateProgram();
    glUseProgram(program);
    CreateVertexShader(program, vert, vert.length());
    CreateFragmentShader(program, frag, frag.length());
    LinkProgram(program);
    glUseProgram(0);
}

FontRenderer::~FontRenderer()
{
    glDeleteProgram(program);

    fonts.clear();

    glDeleteVertexArrays(1, &vao.handle);
    glDeleteBuffers(vao.buffers.size(), &vao.buffers[0]);
}

bool FontRenderer::LoadFTFace(const std::string& key, const std::string& filepath, uint fontSize)
{
    if (fonts.count(key) > 0)
    {
        fmt::print("[TIDE] Font {} already present\n", key);
        return false;
    }
    Shared<Font> font = std::make_shared<Font>();
    int error = FT_New_Face(library, filepath.c_str(), 0, &font->ftFace);
    if (error > 0)
    {
        fmt::print("[TIDE] {} {}\n", error, FT_Error_String(error));
    }
    font->fontSize = fontSize;
    font->fontPath = filepath;
    FT_Set_Pixel_Sizes(font->ftFace, 0, fontSize);
    font->descender = font->ftFace->size->metrics.descender >> 6;
    font->ascender = font->ftFace->size->metrics.ascender >> 6;
    font->assigned = true;
    fonts.insert(std::make_pair(key, font));
    return true;
}

bool FontRenderer::LoadFontGlyphs(const std::string& key)
{
    if (fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to load glyphs of a font ({}) that doesn't exist\n", key);
        return false;
    }

    Shared<Font>& font = fonts.at(key);
    font->maxWidth = 0;
    font->maxHeight = 0;

    uint gindex;
    ulong charcode;

    charcode = FT_Get_First_Char(font->ftFace, &gindex);
    while (gindex != 0)
    {
        uint error = FT_Load_Char(font->ftFace, charcode, FT_LOAD_RENDER);
        if (error)
        {
            fmt::print("[TIDE] Error loading character {}\n", gindex);
        }
        if (font->ftFace->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
        {
            fmt::print("[TIDE] Error where character {} is the incorrect pixel mode ({})\n", charcode, font->ftFace->glyph->bitmap.pixel_mode);
        }

        Glyph* glyph = new Glyph;

        glyph->bitmap = (uchar*)malloc(sizeof(uchar) * font->ftFace->glyph->bitmap.width * font->ftFace->glyph->bitmap.rows);
        memcpy(glyph->bitmap, font->ftFace->glyph->bitmap.buffer, sizeof(unsigned char) * font->ftFace->glyph->bitmap.width * font->ftFace->glyph->bitmap.rows);
        glyph->width = font->ftFace->glyph->bitmap.width;
        glyph->height = font->ftFace->glyph->bitmap.rows;
        glyph->advance = font->ftFace->glyph->advance.x / 64.0;
        glyph->bearingX = font->ftFace->glyph->metrics.horiBearingX / 64.0;
        glyph->bearingY = font->ftFace->glyph->metrics.horiBearingY / 64.0;
        glyph->charcode = charcode;
        font->glyphs.insert(std::make_pair(gindex, glyph));

        if ((int)font->ftFace->glyph->bitmap.width > font->maxWidth)
        {
            font->maxWidth = font->ftFace->glyph->bitmap.width;
        }
        if ((int)font->ftFace->glyph->bitmap.rows > font->maxHeight)
        {
            font->maxHeight = font->ftFace->glyph->bitmap.rows;
        }
        charcode = FT_Get_Next_Char(font->ftFace, charcode, &gindex);
    }
    return true;
}

bool FontRenderer::GenerateFontTextures(const std::string& key)
{
    if (fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to create glyph textures of a font ({}) that doesn't exist\n", key);
        return false;
    }
    Shared<Font>& font = fonts.at(key);
    // Delete the previous texture if it exists.
    if (font->assignedGL)
    {
        glDeleteTextures(1, &font->textureHandleGL);
    }

    // Generates a texture and stores the bitmaps in an OpenGL Texture Array
    glGenTextures(1, &font->textureHandleGL);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font->textureHandleGL);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    SetMinMag(GL_TEXTURE_2D_ARRAY, GL_LINEAR, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
    SetTextureArrayStorage(font->textureHandleGL, GL_R8, font->maxWidth, font->maxHeight, font->glyphs.size());
    for (std::unordered_map<uint, Glyph*>::iterator it = font->glyphs.begin(); it != font->glyphs.end(); it++)
    {
        LoadTextureArrayLayer(font->textureHandleGL, it->second->bitmap, 0, 0, it->second->width, it->second->height, it->first, GL_RED);
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    font->assignedGL = true;
    return true;
}

bool FontRenderer::LoadFace(const std::string& key, const std::string& filepath, int fontSize)
{
    if (LoadFTFace(key, filepath, fontSize))
    {
        if (LoadFontGlyphs(key))
        {
            if (GenerateFontTextures(key))
            {
                return true;
            }
        }
    }
    return false;
}

bool FontRenderer::DeriveFace(const std::string& key, uint newFontSize)
{
    if (fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to derive from a font ({}) that doesn't exist\n", key);
        return false;
    }
    const Shared<Font>& font = fonts.at(key);

    fonts.erase(key);

    if (LoadFTFace(key, font->fontPath, newFontSize))
    {
        if (LoadFontGlyphs(key))
        {
            if (GenerateFontTextures(key))
            {
                return true;
            }
        }
    }
    return false;
}

bool FontRenderer::CloneFace(const std::string& key, const std::string& newKey, uint newFontSize)
{
    if (fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to clone from a font ({}) that doesn't exist\n", key);
        return false;
    }
    if (fonts.count(newKey) != 0)
    {
        fmt::print("[TIDE] Unable to replace an existing font ({}) with a derived font\n", key);
        return false;
    }
    const Shared<Font>& font = fonts.at(key);
    if (LoadFTFace(newKey, font->fontPath, newFontSize))
    {
        if (LoadFontGlyphs(newKey))
        {
            if (GenerateFontTextures(newKey))
            {
                return true;
            }
        }
    }
    return false;
}

void FontRenderer::RenderCursor(float x, float y, float width, float height, float layer, const Color& color)
{
    glUseProgram(program);
    glBindVertexArray(vao.handle);

    glm::mat4 projection = glm::ortho<float>(0.0, engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight(), 0.0, 0.0001, 10.0);
    UniformMat4(program, "projection", projection);

    glm::vec3 translate(x, y, 0.0);
    glm::vec3 scale(width, height, 1.0);
    glm::mat4 transformation(1.0f);
    transformation = glm::translate(transformation, translate);
    transformation = glm::scale(transformation, scale);
    UniformMat4(program, "transformation", transformation);

    UniformVec3(program, "color", color.vec);

    glUniform1i(glGetUniformLocation(program, "caret"), true);
    glUniform1f(glGetUniformLocation(program, "defaultLayerZ"), -layer);
    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindVertexArray(0);
    glUseProgram(0);
}

bool FontRenderer::RenderText(const std::string& key, const std::string& text, int screenX, int screenY, float layer, const Color& color)
{
    return RenderText(key, text, screenX, screenY, layer, color, glm::vec2(0, INT32_MAX));
}

bool FontRenderer::RenderText(const std::string& key, const std::string& text, int screenX, int screenY, float layer, const Color& color, const glm::vec2& ssHorizontalCuttoff)
{
    if (fonts.count(key) == 0)
    {
        // fmt::print("[TIDE] Unable to render a font ({}) that doesn't exist\n", key);
        return false;
    }
    if(text.empty())
        return true;
    const Shared<Font>& font = fonts.at(key);
    // Binds
    glUseProgram(program);
    glBindVertexArray(vao.handle);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font->textureHandleGL);

    UniformVec2(program, "ssHorizontalCuttoff", ssHorizontalCuttoff);

    // Color matrix uniform
    UniformVec3(program, "color", color.vec);

    // Orthographic matrix uniform
    glm::mat4 projection = glm::ortho<float>(0.0, engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight(), 0.0, 0.0001, 10.0);
    UniformMat4(program, "projection", projection);
    glUniform1i(glGetUniformLocation(program, "caret"), false);

    float caret = 0.0;
    glm::vec3 scale((float)font->maxWidth, (float)font->maxHeight, 1.0);

    for (uint i = 0; i < text.length(); i++)
    {
        if (text[i] == '\t')
        {
            caret += font->glyphs.at(FT_Get_Char_Index(font->ftFace, ' '))->advance * 4;
            continue;
        }

        uint charInd = FT_Get_Char_Index(font->ftFace, text[i]);
        if (font->glyphs.find(charInd) == font->glyphs.end())
        {
            continue;
        }
        const Glyph& ch = *font->glyphs.at(charInd);

        float microlayer = (float)(i + 1) / (text.length() + 1);
        glUniform1f(glGetUniformLocation(program, "defaultLayerZ"), -layer + microlayer);

        float hb = font->ascender - ch.bearingY - 1;

        float advance = ch.advance;
        float bearingX = ch.bearingX;

        glm::vec3 translate((float)screenX + (caret + bearingX), (float)screenY + hb, 0.0);

        glm::mat4 transformation(1.0f);
        transformation = glm::translate(transformation, translate);
        transformation = glm::scale(transformation, scale);

        UniformMat4(program, "transformation", transformation);

        glUniform1i(glGetUniformLocation(program, "layer"), FT_Get_Char_Index(font->ftFace, text[i]));

        // Drawing
        glDrawArrays(GL_TRIANGLES, 0, 12);
        caret += advance;
    }

    // Unbinds
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

float FontRenderer::TextWidth(const std::string& key, const std::string& text)
{
    if (fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to gauge the text width of a font ({}) that doesn't exist\n", key);
        return -1;
    }
    const Shared<Font>& font = fonts.at(key);

    float width = 0;
    for (uint i = 0; i < text.length(); i++)
    {
        if (text[i] == '\t')
        {
            width += font->glyphs.at(FT_Get_Char_Index(font->ftFace, ' '))->advance * 4;
            continue;
        }
        uint charInd = FT_Get_Char_Index(font->ftFace, text[i]);
        if (font->glyphs.find(charInd) == font->glyphs.end())
        {
            continue;
        }
        Glyph* ch = font->glyphs.at(charInd);

        width += ch->advance;
    }
    return width;
}

Shared<Font> FontRenderer::GetFont(const std::string& key)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Couldn't retrieve font ({}) as it doesn't exist\n", key);
        return nullptr;
    }
    return fonts.at(key);
}