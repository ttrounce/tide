#ifndef VHM_FONT_H
#define VHM_FONT_H

#include "../types.h"
#include "gfx.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

extern FT_Library library;

struct Glyph
{
    uchar* bitmap;
    char charcode;
    float width;
    float height;
    float advance;
    float bearingX;
    float bearingY;
};

struct CharacterInstance
{
    glm::vec2 pos;
    glm::vec2 size;
    int textureIndex;
    float layer;
    glm::vec3 color;
};

struct Font
{
    // map of glyph metrics+bitmap
    std::unordered_map<uint, Glyph*> glyphs;

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

    float ascender;
    float descender;

    ~Font();
};

using InstanceBatch = std::vector<CharacterInstance>;

void InitFreeType();
void FreeFreeType();

class FontRenderer
{
public:
    FontRenderer();
    ~FontRenderer();
    /**
         * Loads a new fontface to the renderer at a certain size & creates the font glyphs.
         */
    bool LoadFace(const std::string& key, const std::string& filepath, int fontSize);
    /**
         * Derives the same face at a different size & creates the new font glyphs.
         */
    bool DeriveFace(const std::string& key, uint newFontSize);

    /**
         * Derives the same face at a different size & stores it as a new font.
         */
    bool CloneFace(const std::string& key, const std::string& newKey, uint newFontSize);

    void UpdateOrthographic(int width, int height);

    void BatchRect(float x, float y, float width, float height, float layer, const Color& color);
    void BatchText(const std::string& key, const std::string& text, int screenX, int screenY, float layer, const Color& Color);
    void Render();
    /**
         * Gauges the width of a string of text using a font in pixels.
         * @returns -1 if the font doesn't exist.
         */
    float TextWidth(const std::string& key, const std::string& text);

    Shared<Font> GetFont(const std::string& key);
private:
    std::unordered_map<std::string, Shared<Font>> fonts;
    std::unordered_map<std::string, InstanceBatch> batchedFonts;
    InstanceBatch cursorInstances;

    GLuint handleVAO;
    GLuint handleProgram;
    GLuint handleBuffer;

    GLuint locationOrthographic;

    std::string fontpath;

    bool LoadFTFace(const std::string& key, const std::string& filepath, uint fontSize);
    bool LoadFontGlyphs(const std::string& key);
    bool GenerateFontTextures(const std::string& key);
};

extern Unique<FontRenderer> fontRenderer;

#endif // VHM_FONT_H