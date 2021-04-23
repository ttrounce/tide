
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

namespace tide
{

FT_Library library;

std::unique_ptr<FONT_RENDERER> fontRenderer;

void InitFreeType()
{
    int error = FT_Init_FreeType(&library);
    if(error)
    {
        fmt::print("[TIDE] Error initialising FreeType\n");
    }
    else
    {
        fmt::print("[TIDE] Successfully initialised FreeType\n");
    }
    fontRenderer = std::make_unique<FONT_RENDERER>();
}

void FreeFreeType()
{
    FT_Done_FreeType(library);
}

FONT_RENDERER::FONT_RENDERER()
{
    // Generate VAO model
    float vertices[12] = {0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0};
    float texels[12] = {0.0, 0.0, 0.99, 0.99, 0.99, 0.0, 0.0, 0.0, 0.0, 0.99, 0.99, 0.99};

    glGenVertexArrays(1, &vao.handle);
    glBindVertexArray(vao.handle);
    SetBuffer(vao, 0, 2, vertices, 12);
    SetBuffer(vao, 1, 2, texels, 12);
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

FONT_RENDERER::~FONT_RENDERER()
{
    glDeleteProgram(program);

    fonts.clear();
    
    glDeleteVertexArrays(1, &vao.handle);
    glDeleteBuffers(vao.buffers.size(), &vao.buffers[0]);
}
    
bool FONT_RENDERER::LoadFTFace(std::string key, std::string filepath, uint fontSize)
{
    if(fonts.count(key) > 0)
    {
        fmt::print("[TIDE] Font {} already present\n", key);
        return false;
    }
    std::shared_ptr<FONT> font = std::make_shared<FONT>();
    int error = FT_New_Face(library, filepath.c_str(), 0, &font->ftFace);
    if(error > 0)
    {
        fmt::print("[TIDE] {} {}\n", error, FT_Error_String(error));
    }
    font->fontSize = fontSize;
    font->fontPath = filepath;
    FT_Set_Pixel_Sizes(font->ftFace, 0, fontSize);
    font->assigned = true;
    fonts.insert(std::make_pair(key, font));
    return true;
}


bool FONT_RENDERER::LoadFontGlyphs(std::string key)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to load glyphs of a font ({}) that doesn't exist\n", key);
        return false;
    }

    std::shared_ptr<FONT> font = fonts.at(key);
    font->maxWidth = 0;
    font->maxHeight = 0;
    font->capitalBearingY = 0;

    uint gindex;
    ulong charcode;

    charcode = FT_Get_First_Char(font->ftFace, &gindex);
    while(gindex != 0)
    {
        uint error = FT_Load_Char(font->ftFace, charcode, FT_LOAD_RENDER);
        if(error)
        {
            fmt::print("[TIDE] Error loading character {}\n", gindex);
        }
        if(font->ftFace->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
        {
            fmt::print("[TIDE] Error where character {} is the incorrect pixel mode ({})\n", charcode, font->ftFace->glyph->bitmap.pixel_mode);
        }

        GLYPH* glyph = new GLYPH;
        glyph->bitmap = (uchar*) malloc(sizeof(uchar) * font->ftFace->glyph->bitmap.width * font->ftFace->glyph->bitmap.rows);
        memcpy(glyph->bitmap, font->ftFace->glyph->bitmap.buffer, sizeof(unsigned char) * font->ftFace->glyph->bitmap.width * font->ftFace->glyph->bitmap.rows);
        glyph->width = font->ftFace->glyph->bitmap.width;
        glyph->height = font->ftFace->glyph->bitmap.rows;
        glyph->advance = font->ftFace->glyph->advance.x / 64.0;
        glyph->bearingX = font->ftFace->glyph->metrics.horiBearingX / 64.0;
        glyph->bearingY = font->ftFace->glyph->metrics.horiBearingY / 64.0;
        glyph->charcode = charcode;
        font->glyphs.insert(std::make_pair(gindex, glyph));

        if(charcode == '[')
        {
            font->capitalBearingY = glyph->bearingY;
        }
        if((int) font->ftFace->glyph->bitmap.width > font->maxWidth)
        {
            font->maxWidth = font->ftFace->glyph->bitmap.width;
        }
        if((int) font->ftFace->glyph->bitmap.rows > font->maxHeight)
        {
            font->maxHeight = font->ftFace->glyph->bitmap.rows;
        }
        charcode = FT_Get_Next_Char(font->ftFace, charcode, &gindex);
    }
    return true;
}

bool FONT_RENDERER::GenerateFontTextures(std::string key)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to create glyph textures of a font ({}) that doesn't exist\n", key);
        return false;
    }
    std::shared_ptr<FONT> font = fonts.at(key);
    // Delete the previous texture if it exists.
    if(font->assignedGL)
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
    for(std::unordered_map<uint, GLYPH*>::iterator it = font->glyphs.begin(); it != font->glyphs.end(); it++)
    {
        LoadTextureArrayLayer(font->textureHandleGL, it->second->bitmap, 0, 0, it->second->width, it->second->height, it->first, GL_RED);
    }
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    font->assignedGL = true;
    return true;
}

bool FONT_RENDERER::LoadFace(std::string key, std::string filepath, int fontSize)
{
    if(LoadFTFace(key, filepath, fontSize))
    {
        if(LoadFontGlyphs(key))
        {
            if(GenerateFontTextures(key))
            {
                return true;
            }
        }
    }
    return false;
}

bool FONT_RENDERER::DeriveFace(std::string key, uint newFontSize)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to derive from a font ({}) that doesn't exist\n", key);
        return false;
    }
    std::shared_ptr<FONT> font = fonts.at(key);

    fonts.erase(key);

    if(LoadFTFace(key, font->fontPath, newFontSize))
    {
        if(LoadFontGlyphs(key))
        {
            if(GenerateFontTextures(key))
            {
                return true;
            }
        }
    }
    return false;
}

bool FONT_RENDERER::CloneFace(std::string key, std::string newKey, uint newFontSize)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to clone from a font ({}) that doesn't exist\n", key);
        return false;
    }
    if(fonts.count(newKey) != 0)
    {
        fmt::print("[TIDE] Unable to replace an existing font ({}) with a derived font\n", key);
        return false;
    }
    std::shared_ptr<FONT> font = fonts.at(key);
    if(LoadFTFace(newKey, font->fontPath, newFontSize))
    {
        if(LoadFontGlyphs(newKey))
        {
            if(GenerateFontTextures(newKey))
            {
                return true;
            }
        }
    }
    return false;
}

void FONT_RENDERER::RenderCursor(float x, float y, float width, float height, float layer, COLOR color)
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

bool FONT_RENDERER::RenderText(std::string key, std::string text, int screenX, int screenY, float layer, COLOR color)
{
    return RenderText(key, text, screenX, screenY, layer, color, glm::vec2(0, INT32_MAX));
}

bool FONT_RENDERER::RenderText(std::string key, std::string text, int screenX, int screenY, float layer, COLOR color, glm::vec2 ssHorizontalCuttoff)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to render a font ({}) that doesn't exist\n", key);
        return false;
    }
    std::shared_ptr<FONT> font = fonts.at(key);
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
    glm::vec3 scale((float) font->maxWidth, (float) font->maxHeight, 1.0);

    for(uint i = 0; i < text.length(); i++)
    {
        if(text[i] == '\t')
        {
            caret += font->glyphs.at(FT_Get_Char_Index(font->ftFace, ' '))->advance * 4;
            continue;
        }

        uint charInd = FT_Get_Char_Index(font->ftFace, text[i]);
        if(font->glyphs.find(charInd) == font->glyphs.end())
        {
            continue;
        }
        GLYPH ch = *font->glyphs.at(charInd);
        
        float microlayer = (float) (i+1) / (text.length() + 1);
        glUniform1f(glGetUniformLocation(program, "defaultLayerZ"), -layer + microlayer);

        float hb = font->capitalBearingY - ch.bearingY - 1;

        float advance = ch.advance;
        float bearingX = ch.bearingX;

        glm::vec3 translate((float) screenX + (caret + bearingX), (float) screenY + hb, 0.0);

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


float FONT_RENDERER::TextWidth(std::string key, std::string text)
{
    if(fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Unable to gauge the text width of a font ({}) that doesn't exist\n", key);
        return -1;
    }
    std::shared_ptr<FONT> font = fonts.at(key);

    float width = 0;
    for(uint i = 0; i < text.length(); i++)
    {
        if(text[i] == '\t')
        {
            width += font->glyphs.at(FT_Get_Char_Index(font->ftFace, ' '))->advance * 4;
            continue;
        }
        uint charInd = FT_Get_Char_Index(font->ftFace, text[i]);
        if(font->glyphs.find(charInd) == font->glyphs.end())
        {
            continue;
        }
        GLYPH* ch = font->glyphs.at(charInd);

        width += ch->advance;
    }
    return width;
}

}