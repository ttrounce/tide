
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
    for (auto e : glyphs)
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
    glGenVertexArrays(1, &handleVAO);
    glGenBuffers(1, &handleBuffer);

    glBindVertexArray(handleVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glBindVertexArray(0);

    // Create Shader Program
    std::string vert = ReadFile("shaders/font_vert.glsl");
    std::string frag = ReadFile("shaders/font_frag.glsl");

    // Create Program
    handleProgram = glCreateProgram();
    glUseProgram(handleProgram);
    CreateVertexShader(handleProgram, vert, vert.length());
    CreateFragmentShader(handleProgram, frag, frag.length());
    LinkProgram(handleProgram);
    glUseProgram(0);

    // Get Uniform Locations
    locationOrthographic = glGetUniformLocation(handleProgram, "orthographic");

    glUseProgram(handleProgram);
    UniformMat4(locationOrthographic, glm::ortho<float>(0.0, engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight(), 0.0, 0.0001, 10.0));
    glUseProgram(0);
    // glm::mat4 projection = glm::ortho<float>(0.0, 800.0, 600.0, 0.0, 0.0001, 10.0);
    // UniformMat4(program, "orthographic", projection);

}

FontRenderer::~FontRenderer()
{
    glDeleteProgram(handleProgram);

    fonts.clear();

    glDeleteVertexArrays(1, &handleVAO);
    glDeleteBuffers(1, &handleBuffer);
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

void FontRenderer::UpdateOrthographic(int width, int height)
{
    glUseProgram(handleProgram);
    UniformMat4(locationOrthographic, glm::ortho<float>(0.0, width, height, 0.0, 0.0001, 10.0));
    glUseProgram(0);
}

void FontRenderer::BatchRect(float x, float y, float width, float height, float layer, const Color& color)
{
    auto instance = CharacterInstance{
        .pos = glm::vec2{x, y},
        .size = glm::vec2{width, height},
        .textureIndex = -1,
        .layer = layer,
        .color = color.vec
    };
    cursorInstances.push_back(instance);
}

void FontRenderer::BatchText(const std::string& key, const std::string& text, int screenX, int screenY, float layer, const Color& color)
{
    if (fonts.count(key) == 0 || text.empty())
    {
        return;
    }

    auto exists = batchedFonts.count(key);
    if (exists == 0)
    {
        batchedFonts.insert(std::pair<std::string, InstanceBatch>(key, {}));
    }
    
    const Shared<Font>& font = fonts.at(key);
    float caret = 0.0;

    auto& fontBatch = batchedFonts.at(key);

    // Load all text into batch...
    for (uint i = 0; i < text.length(); i++)
    {
        // White space checker
        switch (text[i])
        {
            case '\t':
            {
                caret += font->glyphs.at(FT_Get_Char_Index(font->ftFace, ' '))->advance * 4;
                continue;
            }
            default: {}
        }

        uint charInd = FT_Get_Char_Index(font->ftFace, text[i]);
        if (font->glyphs.find(charInd) == font->glyphs.end())
        {
            continue;
        }
        const Glyph& ch = *font->glyphs.at(charInd);

        auto instance = CharacterInstance{
            .pos = glm::vec2{screenX + (caret + ch.bearingX), screenY + (font->ascender - ch.bearingY - 1)},
            .size = glm::vec2{font->maxWidth, font->maxHeight},
            .textureIndex = (int)charInd,
            .layer = layer,
            .color = color.vec
        };
        fontBatch.push_back(instance);

        caret += ch.advance;
    }
}

void FontRenderer::Render()
{
    glUseProgram(handleProgram);
    glBindVertexArray(handleVAO);

    if(!cursorInstances.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, handleBuffer);
        glBufferData(GL_ARRAY_BUFFER, cursorInstances.size() * sizeof(CharacterInstance), cursorInstances.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, pos         ));
        glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, size        ));
        glVertexAttribIPointer(2, 1, GL_INT  ,           sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, textureIndex));
        glVertexAttribPointer (3, 1, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, layer       ));
        glVertexAttribPointer (4, 3, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, color       ));

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, cursorInstances.size());
    }
    for (auto fontBatch : batchedFonts)
    {
        if (fonts.count(fontBatch.first) == 0)
            continue;

        const auto& font = fonts.at(fontBatch.first);
        const auto& batch = fontBatch.second;

        if (batch.empty())
            continue;

        glBindTexture(GL_TEXTURE_2D_ARRAY, font->textureHandleGL);
        glBindBuffer(GL_ARRAY_BUFFER, handleBuffer);
        glBufferData(GL_ARRAY_BUFFER, batch.size() * sizeof(CharacterInstance), batch.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, pos         ));
        glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, size        ));
        glVertexAttribIPointer(2, 1, GL_INT  ,           sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, textureIndex));
        glVertexAttribPointer (3, 1, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, layer       ));
        glVertexAttribPointer (4, 3, GL_FLOAT, GL_FALSE, sizeof(CharacterInstance), (void*) offsetof(CharacterInstance, color       ));

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, batch.size());
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    batchedFonts.clear();
    cursorInstances.clear();
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
    if (fonts.count(key) == 0)
    {
        fmt::print("[TIDE] Couldn't retrieve font ({}) as it doesn't exist\n", key);
        return nullptr;
    }
    return fonts.at(key);
}