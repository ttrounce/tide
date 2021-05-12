#ifndef TIDE_FONT_PARAMS_H
#define TIDE_FONT_PARAMS_H

#include "../engine/gfx/font.h"
#include "../engine/types.h"

#include <string>

struct FontParams {
    std::string fontName;
    int linePadding;
    int GetFontSize() const;
    int GetLineHeight() const;
};

#endif // TIDE_FONT_PARAMS_H