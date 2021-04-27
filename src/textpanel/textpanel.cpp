#include "textpanel.h"

#include <fmt/core.h>

TextPanel::TextPanel(const Rect& rect) : panelRectangle(rect)
{
    cursor = { 0, 0 };
    camera = { 0, 0 };
    fontParameters = { "hack", 2 };
    isFocused = false;
    lines.push_back(std::string());
}

Bounds TextPanel::GetContentBounds()
{
    const int fontSize = fontParameters.GetFontSize();
    int x1 = panelRectangle.x + (glm::floor(log10(lines.size())) + 1) * fontSize + (fontSize / 2.0);
    int y1 = panelRectangle.y;
    int x2 = panelRectangle.x + panelRectangle.w;
    int y2 = panelRectangle.y + panelRectangle.h - fontParameters.GetLineHeight();
    return Bounds(x1, y1, x2, y2);
}

glm::ivec2 TextPanel::GetCursorPixelPosition()
{
    std::string leftText = lines.at(cursor.y).substr(0, cursor.x);
    int x = GetContentBounds().x1 + camera.x + fontRenderer->TextWidth(fontParameters.fontName, leftText);
    int y = GetContentBounds().y1 + camera.y + cursor.y * fontParameters.GetLineHeight();
    return glm::ivec2(x, y);
}

glm::ivec2 TextPanel::GetPixelShiftBorder()
{
    const int fontSize = fontParameters.GetFontSize();
    return glm::vec2(GetContentBounds().x1 + 2 * fontSize, GetContentBounds().x2 - 2 * fontSize);
}

void TextPanel::TakeInput(InputType type, uint codepoint)
{
    if (!isFocused)
        return;
    // used to pipeline all text-panel input interaction
    int removeBackPos = -1;
    switch (type)
    {
    case INPUT_WRITE:
        WriteChar(codepoint);
    case INPUT_MOVE_RIGHT:
        MoveCursorRight();
        break;
    case INPUT_REMOVE_BACK:
        removeBackPos = RemoveBackwardChar();
    case INPUT_MOVE_LEFT:
        MoveCursorLeft(removeBackPos);
        break;
    case INPUT_NEW_LINE:
        NewLine();
        break;
    case INPUT_MOVE_UP:
        MoveCursorUp();
        break;
    case INPUT_MOVE_DOWN:
        MoveCursorDown();
        break;
    case INPUT_REMOVE_FWRD:
        RemoveForwardChar();
        break;
    case INPUT_SCROLL_UP:
    case INPUT_SCROLL_DOWN:
    case INPUT_SHIFT_LEFT:
    case INPUT_SHIFT_RIGHT:
        break;
    }

    const int fontSize = fontParameters.GetFontSize();

    // after any type of input, we reposition the camera so that the cursor
    // is either as left as possible or as right as possible.
    if (GetCursorPixelPosition().x >= GetPixelShiftBorder().y)
    {
        camera.x = -fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(0, cursor.x)) + (GetContentBounds().x2 - GetContentBounds().x1) - 2 * fontSize;
    }
    else if (cursor.x > 3 && GetCursorPixelPosition().x < GetPixelShiftBorder().x)
    {
        camera.x = -fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(0, cursor.x)) + 2 * fontSize;
    }
    else if (cursor.x <= 3)
    {
        camera.x = 0;
    }

    if (GetCursorPixelPosition().y < GetContentBounds().y1)
    {
        camera.y = -fontParameters.GetLineHeight() * cursor.y;
    }

    if (GetCursorPixelPosition().y > GetContentBounds().y2 || GetCursorPixelPosition().y + fontParameters.GetLineHeight() >= GetContentBounds().y2)
    {
        camera.y = -fontParameters.GetLineHeight() * cursor.y + (GetContentBounds().y2 - GetContentBounds().y1) - fontParameters.GetLineHeight();
    }
}


void TextPanel::WriteChar(uint codepoint)
{
    lines.at(cursor.y).insert(cursor.x, 1, codepoint);
}

void TextPanel::RemoveForwardChar()
{
    std::string line = lines.at(cursor.y);
    if (cursor.x < static_cast<int>(line.length()))
        lines.at(cursor.y) = line.substr(0, cursor.x) + line.substr(cursor.x + 1, line.length());
    else if (cursor.y < static_cast<int>(lines.size()) - 1)
    {
        lines.at(cursor.y) += lines.at(cursor.y + 1);
        lines.erase(lines.begin() + cursor.y + 1);
    }
}

int TextPanel::RemoveBackwardChar()
{
    std::string line = lines.at(cursor.y);
    if (cursor.x > 0)
        lines.at(cursor.y) = line.substr(0, cursor.x - 1) + line.substr(cursor.x, line.length());
    else if (cursor.y > 0)
    {
        int originalSize = lines.at(cursor.y - 1).length();
        lines.at(cursor.y - 1) = lines.at(cursor.y - 1) + line;
        lines.erase(lines.begin() + cursor.y);
        return originalSize;
    }
    return -1;
}

void TextPanel::NewLine()
{
    std::string line = lines.at(cursor.y);
    if (cursor.x == static_cast<int>(line.length()))
    {
        lines.insert(lines.begin() + cursor.y + 1, std::string());
    }
    else
    {
        lines.at(cursor.y) = line.substr(0, cursor.x);
        lines.insert(lines.begin() + cursor.y + 1, line.substr(cursor.x, line.length()));
    }
    cursor.x = 0;
    cursor.y += 1;
}

void TextPanel::MoveCursorUp()
{
    if (cursor.y > 0)
        cursor.y -= 1;
    cursor.x = glm::min<int>(lines.at(cursor.y).length(), cursor.x);
}

void TextPanel::MoveCursorDown()
{
    if (cursor.y < static_cast<int>(lines.size()) - 1)
        cursor.y += 1;
    cursor.x = glm::min<int>(lines.at(cursor.y).length(), cursor.x);
}

void TextPanel::MoveCursorLeft(int cursorX)
{
    if (cursor.x > 0)
        cursor.x -= 1;
    else if (cursor.y > 0)
    {
        cursor.x = cursorX >= 0 ? cursorX : lines.at(cursor.y - 1).length();
        cursor.y -= 1;
    }
}

void TextPanel::MoveCursorRight()
{
    if (cursor.x < static_cast<int>(lines.at(cursor.y).length()))
        cursor.x += 1;
    else if (cursor.y < static_cast<int>(lines.size()) - 1)
    {
        cursor.x = 0;
        cursor.y += 1;
    }
}

void TextPanel::Render()
{
    int lineNumber = 0;

    int offsetLnNumbers = (glm::floor(log10(lines.size())) + 1) * fontParameters.GetFontSize();

    int totalLineHeight = fontParameters.GetLineHeight();

    for (auto it = lines.begin(); it != lines.end(); it++, lineNumber++)
    {
        std::string& originalLine = *it;

        int lineY = GetContentBounds().y1 + (lineNumber * totalLineHeight) + camera.y;
        int textY = fontParameters.linePadding + lineY;
        int textX = GetContentBounds().x1 + camera.x;

        // cull lines outside of content bounds
        if (lineY < GetContentBounds().y1 || lineY >= GetContentBounds().y2 || lineY + fontParameters.GetLineHeight() > GetContentBounds().y2)
            continue;

        // Render panel header
        fontRenderer->RenderCursor(panelRectangle.x, GetContentBounds().y2, panelRectangle.w, fontParameters.GetLineHeight(), 1.0, Color(0xFFFFFF));
        fontRenderer->RenderText(fontParameters.fontName, "textpanel2.cpp", panelRectangle.x, GetContentBounds().y2 + fontParameters.linePadding, 2, Color(0x000000));

        // Render the base line-selector first for transparency. 
        std::string lineString = fmt::format("{}", lineNumber + 1);
        if (lineNumber == cursor.y && isFocused)
        {
            // Render line-selector
            fontRenderer->RenderCursor(GetContentBounds().x1, textY - fontParameters.linePadding, panelRectangle.w - GetContentBounds().x1, totalLineHeight, 1, Color(0x141414));
            // Render the line number, with the highlight
            fontRenderer->RenderText(fontParameters.fontName, lineString, panelRectangle.x + offsetLnNumbers - fontRenderer->TextWidth(fontParameters.fontName, lineString), textY, 2, Color(0xFF0000));
        }
        else
        {
            // Render the line number, without the highlight
            fontRenderer->RenderText(fontParameters.fontName, lineString, panelRectangle.x + offsetLnNumbers - fontRenderer->TextWidth(fontParameters.fontName, lineString), textY, 2, Color(0x444444));
        }

        // Render the line text
        fontRenderer->RenderText(fontParameters.fontName, originalLine, textX, textY, 2, Color(0xFFFFFF), glm::vec2(GetContentBounds().x1, GetContentBounds().x2));

        // Render the cursor on top of everything.
        if (lineNumber == cursor.y && isFocused)
        {
            std::string leftText = (originalLine).substr(0, cursor.x);

            int cursorRenderX = textX + fontRenderer->TextWidth(fontParameters.fontName, leftText);
            int cursorRenderY = textY - fontParameters.linePadding;
            fontRenderer->RenderCursor(cursorRenderX, cursorRenderY, fontParameters.GetFontSize() / 8.0, totalLineHeight, 3, Color(0xFFFFFF));
        }
    }
}