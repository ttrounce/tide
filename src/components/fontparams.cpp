#include "fontparams.h"

int FontParams::GetFontSize() const
{
    const Shared<Font>& fnt = fontRenderer->GetFont(this->fontName);
    if(fnt)
    {
        return fnt->fontSize;
    }
    return 0;
}

int FontParams::GetLineHeight() const
{
    const Shared<Font>& fnt = fontRenderer->GetFont(this->fontName);
    if(fnt)
    {
        return (2 * this->linePadding + (int)(fnt->ascender - fnt->descender));
    }
    return 0;
}