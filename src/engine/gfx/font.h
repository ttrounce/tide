#ifndef VHM_FONT_H
#define VHM_FONT_H

#include "../types.h"
#include "gfx.h"

#include <map>
#include <memory>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

extern FT_Library library;

namespace tide
{

struct GLYPH
{
    uchar* bitmap;
    char charcode;
    float width;
    float height;
    float advance;
    float bearingX;
    float bearingY;
};

struct FONT
{
    // map of glyph metrics+bitmap
    std::map<uint, GLYPH*> glyphs;

    // FreeType
    FT_Face ftFace;
    bool assigned;
    std::string fontPath;

    // opengl
    bool assignedGL;
    GLuint textureHandleGL;

    int fontSize;
    
    // size specs
    int maxWidth;
    int maxHeight;
    int capitalBearingY;

    ~FONT()
    {
        FT_Done_Face(ftFace);
        for(auto it = glyphs.begin(); it != glyphs.end(); it++)
        {
            free(it->second->bitmap);
            delete it->second;
        }
        glyphs.clear();
        glDeleteTextures(1, &textureHandleGL);
    }
};

void InitFreeType();
void FreeFreeType();

class FONT_RENDERER
{

    private:
        std::map<std::string, std::shared_ptr<FONT>> fonts;

        VAO    vao;
        GLuint program;
        std::string  fontpath;

        bool LoadFTFace(std::string key, std::string filepath, uint fontSize);
        bool LoadFontGlyphs(std::string key);
        bool GenerateFontTextures(std::string key);

    public:
        FONT_RENDERER();
        ~FONT_RENDERER();
        void RenderCursor(float x, float y, float width, float height, COLOR color);
        /**
         * Loads a new fontface to the renderer at a certain size & creates the font glyphs.
         */
        bool LoadFace(std::string key, std::string filepath, int fontSize);
        /**
         * Derives the same face at a different size & creates the new font glyphs.
         */
        bool DeriveFace(std::string key, uint newFontSize);

        /**
         * Derives the same face at a different size & stores it as a new font.
         */
        bool CloneFace(std::string key, std::string newKey, uint newFontSize);

        bool RenderText(std::string key, std::string text, int screenX, int screenY, COLOR color);
        bool RenderText(std::string key, std::string text, int screenX, int screenY, COLOR color, glm::vec2 screenspaceHorizontalCuttoff);
        /**
         * Gauges the width of a string of text using a font in pixels.
         * @returns -1 if the font doesn't exist.
         */
        float TextWidth(std::string key, std::string text);
};

extern std::unique_ptr<FONT_RENDERER> fontRenderer;

}

#endif // VHM_FONT_H