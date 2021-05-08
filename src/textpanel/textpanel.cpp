#include "textpanel.h"

#include <fmt/core.h>

TextPanel::TextPanel(const Rect& rect, const TextPanelParameters& params) : panelRectangle(rect), panelParameters(params)
{
    cursor = { 0, 0 };
    camera = { 0, 0 };
    isFocused = false;
    lines.push_back(std::string());
}

Bounds TextPanel::GetContentBounds()
{
    const int fontSize = panelParameters.GetFontSize();
    int x1 = panelRectangle.x + (glm::floor(log10(lines.size())) + 1) * fontSize + (fontSize / 2.0);
    int y1 = panelRectangle.y + panelParameters.GetLineHeight();
    int x2 = panelRectangle.x + panelRectangle.w;
    int y2 = panelRectangle.y + panelRectangle.h;
    return Bounds(x1, y1, x2, y2);
}

glm::ivec2 TextPanel::GetCursorPixelPosition()
{
    std::string leftText = lines.at(cursor.y).substr(0, cursor.x);
    int x = GetContentBounds().x1 + camera.x + fontRenderer->TextWidth(panelParameters.fontName, leftText);
    int y = GetContentBounds().y1 + camera.y + cursor.y * panelParameters.GetLineHeight();
    return glm::ivec2(x, y);
}

glm::ivec2 TextPanel::GetPixelShiftBorder()
{
    const int fontSize = panelParameters.GetFontSize();
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
        [[fallthrough]]
    case INPUT_MOVE_RIGHT:
        MoveCursorRight();
        break;
    case INPUT_REMOVE_BACK:
        removeBackPos = RemoveBackwardChar();
        [[fallthrough]]
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

    const int fontSize = panelParameters.GetFontSize();

    // after any type of input, we reposition the camera so that the cursor
    // is either as left as possible or as right as possible.
    if (GetCursorPixelPosition().x >= GetPixelShiftBorder().y)
    {
        camera.x = -fontRenderer->TextWidth(panelParameters.fontName, lines.at(cursor.y).substr(0, cursor.x)) + (GetContentBounds().x2 - GetContentBounds().x1) - 2 * fontSize;
    }
    else if (cursor.x > 3 && GetCursorPixelPosition().x < GetPixelShiftBorder().x)
    {
        camera.x = -fontRenderer->TextWidth(panelParameters.fontName, lines.at(cursor.y).substr(0, cursor.x)) + 2 * fontSize;
    }
    else if (cursor.x <= 3)
    {
        camera.x = 0;
    }

    if (GetCursorPixelPosition().y < GetContentBounds().y1)
    {
        camera.y = -panelParameters.GetLineHeight() * cursor.y;
    }

    if (GetCursorPixelPosition().y > GetContentBounds().y2 || GetCursorPixelPosition().y + panelParameters.GetLineHeight() >= GetContentBounds().y2)
    {
        camera.y = -panelParameters.GetLineHeight() * cursor.y + (GetContentBounds().y2 - GetContentBounds().y1) - panelParameters.GetLineHeight();
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

#include <iostream>
#include <chrono>
#include <fmt/chrono.h>

void TextPanel::Batch()
{
    const Bounds& contentBounds = GetContentBounds();
    const int totalLineHeight = panelParameters.GetLineHeight();
    const int totalLineCount = lines.size();

    int offsetLnNumbers = (glm::floor(log10(totalLineCount)) + 1) * panelParameters.GetFontSize();

    int lineStart = glm::max<int>(0, (-contentBounds.y1 - camera.y) / totalLineHeight);
    int lineEnd = glm::min<int>(totalLineCount, ((contentBounds.y2 - contentBounds.y1) - camera.y) / totalLineHeight);
    int lineNumber = lineStart;

    if(isFocused)
    {
        fontRenderer->BatchRect(panelRectangle.x, panelRectangle.y, panelRectangle.w, panelRectangle.h, 10.0, Color(0x0A0A0A));
    }

    // fontRenderer->BatchRect(panelRectangle.x, contentBounds.y2, panelRectangle.w, totalLineHeight, 1.0, Color(0xFFFFFF));
    fontRenderer->BatchText(panelParameters.fontName, "Untitled", panelRectangle.x + 3, panelRectangle.y + panelParameters.linePadding + 1, 5.0, Color(0x777777));

    for(auto i = lines.begin() + lineStart; i != lines.begin() + lineEnd; i++, lineNumber++)
    {
        const auto& line = *i;

        int lineY = contentBounds.y1 + (lineNumber * totalLineHeight) + camera.y;
        int textY = panelParameters.linePadding + lineY;
        int textX = GetContentBounds().x1 + camera.x;

        // cull lines outside of content bounds
        if (lineY < contentBounds.y1 || lineY >= contentBounds.y2 || lineY + totalLineHeight > contentBounds.y2)
            continue;

        // // Render panel header

        // // Render the base line-selector first for transparency. 
        std::string lineString = fmt::format("{}", lineNumber + 1);
        if (lineNumber == cursor.y && isFocused)
        {
            // Render line-selector
            fontRenderer->BatchRect(contentBounds.x1, textY - panelParameters.linePadding, panelRectangle.w - contentBounds.x1, totalLineHeight, 1, Color(0x141414));
            // Render the line number, with the highlight
            fontRenderer->BatchText(panelParameters.fontName, lineString, panelRectangle.x + offsetLnNumbers - fontRenderer->TextWidth(panelParameters.fontName, lineString), textY, 2, Color(0xFFFFFF));
        }
        else
        {
            // Render the line number, without the highlight
            fontRenderer->BatchText(panelParameters.fontName, lineString, panelRectangle.x + offsetLnNumbers - fontRenderer->TextWidth(panelParameters.fontName, lineString), textY, 2, Color(0x444444));
        }
        
        // Render the line text
        fontRenderer->BatchText(panelParameters.fontName, line, textX, textY, 0.1, Color(0xFFFFFF)/*, glm::vec2(GetContentBounds().x1, GetContentBounds().x2)*/);

        // Render the cursor on top of everything.
        if (lineNumber == cursor.y && isFocused)
        {
            std::string leftText = line.substr(0, cursor.x);

            int cursorRenderX = textX + fontRenderer->TextWidth(panelParameters.fontName, leftText);
            int cursorRenderY = textY - panelParameters.linePadding;
            fontRenderer->BatchRect(cursorRenderX, cursorRenderY, panelParameters.GetFontSize() / 8.0, totalLineHeight, 1.0, Color(0xFFFFFF));
        }
    }

    fontRenderer->BatchRect(panelRectangle.x, panelRectangle.y + totalLineHeight, panelRectangle.w, 1, 1.0, Color(0x444444));
}