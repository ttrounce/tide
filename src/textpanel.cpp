#include "textpanel.h"
#include "engine/gfx/font.h"
#include "engine/engine.h"

#include <iostream>

namespace tide
{

TEXT_PANEL::TEXT_PANEL(glm::vec2 pos, glm::vec2 dim) : pos(pos), dim(dim)
{
    cursor = {0, 0};
    camera = {0, 0};
    scrollOffset = {0, 0};
    fontParameters = {"hack", 16, 2};
    lines.push_back(std::string());
}

glm::ivec2 TEXT_PANEL::GetScrollBoundsVertical()
{
    glm::ivec2 base(0, (int) glm::floor<int>(dim.y / fontParameters.GetLineHeight()));
    return base + scrollOffset.y;
}


void TEXT_PANEL::WriteChar(uint codepoint)
{
    if(isFocused)
    {   
        // Inserts a character at a given cursor.x
        std::string line = lines.at(cursor.y);
        lines.at(cursor.y).insert(cursor.x, 1, codepoint);
        cursor.x += 1;
        float textWidth = fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y));
        if(GetCursorPixelX() > GetPixelShiftBorder().y)
        {
            camera.x -= fontRenderer->TextWidth(fontParameters.fontName, std::string().insert(0, 1, codepoint));
        }
    }
}

void TEXT_PANEL::RemoveForwardChar()
{
    if(isFocused)
    {
        if(lines.size() > 0)
        {
            std::string line = lines.at(cursor.y);
            if(cursor.x == line.size())
            {
                // Rule for when the cursor is at the end of a line
                if(cursor.y + 1 < lines.size())
                {
                    // If there is another line below, remove the line break and merge the lines.
                    lines.at(cursor.y) += lines.at(cursor.y + 1);
                    lines.erase(lines.begin() + cursor.y + 1);
                }
            }
            else
            {
                // Rule for when the cursor is anywhere else in the line.
                lines.at(cursor.y) = line.substr(0, cursor.x) + line.substr(cursor.x + 1, line.size());
            }
        }
    }
}


void TEXT_PANEL::RemoveBackwardChar()
{
    if(isFocused)
    {
        if(lines.size() > 0)
        {
            std::string line = lines.at(cursor.y);
            if(cursor.x > 0 && !line.empty())
            {
                int gcpx = GetCursorPixelX();
                int gtsx = GetTextStartX();
                if(GetCursorPixelX()  <= GetPixelShiftBorder().x && camera.x <= 0)
                {
                    camera.x += fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(cursor.x - 1, 1));
                    if(camera.x > 0)
                        camera.x = 0;
                }
                // If cursor isn't at the start of the line & the line isn't empty
                // remove the previous character.
                lines.at(cursor.y) = line.substr(0, cursor.x - 1) + line.substr(glm::min<int>(cursor.x, line.size()), line.size());
                cursor.x -= 1;
            }
            else
            {
                // If the cursor is on the first line or an empty line
                if(cursor.y >= 1)
                {
                    // We're not on the first line (in which do nothing)
                    if(line.empty())
                    {   
                        // We're on an empty line, hence erase the line and move the cursor up
                        // to the end of the line above.
                        lines.erase(lines.begin() + cursor.y);
                        cursor -= glm::vec2(0, 1);
                        cursor.x = lines.at(cursor.y).size();
                        float textWidth = fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y));
                        if(textWidth < dim.x)
                        {
                            // reset camera to the start if the line text-width is smaller than the textpanel.
                            camera.x = 0;
                        }
                        else
                        {
                            // reset the camera so the last character is on the far-right side of the textpanel.
                            // camera.x = panel width - text starting position - text width - 1x the font size
                            camera.x = dim.x - GetTextStartX() - textWidth - fontParameters.fontSize;
                        }
                        if(cursor.y < GetScrollBoundsVertical().x)
                        {
                            ScrollBoundsUp();
                        }
                    }
                    else
                    {
                        // We're not on an empty line, hence merge the line above with the
                        // contents of this line, and remove this line, moving the cursor up
                        cursor -= glm::vec2(0, 1);
                        cursor.x = lines.at(cursor.y).size();
                        float textWidth = fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y));
                        lines.at(cursor.y) += line;
                        lines.erase(lines.begin() + cursor.y + 1);
                        if(textWidth < dim.x)
                        {
                            // reset camera to the start if the line text-width is smaller than the textpanel.
                            camera.x = 0;
                        }
                        else
                        {
                            // reset the camera so the last character is on the far-right side of the textpanel.
                            // camera.x = panel width - text starting position - text width - 1x the font size
                            camera.x = dim.x - GetTextStartX() - textWidth - fontParameters.fontSize;
                        }
                    }
                }
            }
        }
    }
}

void TEXT_PANEL::NewLine()
{
    if(isFocused)
    {   
        // Split the current line and insert it as a new line, then move the cursor
        // down to the beginning of the new line
        std::string line = lines.at(cursor.y);
        std::string rightText = line.substr(cursor.x, line.size());
        lines.at(cursor.y) = line.substr(0, cursor.x);
        lines.insert(lines.begin() + cursor.y + 1, rightText);
        MoveCursorDown();
        cursor.x = 0;
        scrollOffset.x = 0;
        camera.x = 0;
        // if(cursor.x < GetHorizontalScrollBounds().x)
        // {
        //     scrollOffset.x = 0;
        // }
    }
}
float TEXT_PANEL::GetTextStartX()
{
    int offsetLnNumbers = (glm::floor(log10(lines.size())) + 1) * fontParameters.fontSize;
    return pos.x + offsetLnNumbers + fontParameters.fontSize;
}

float TEXT_PANEL::GetCursorPixelX()
{
    std::string leftText = lines.at(cursor.y).substr(0, cursor.x - scrollOffset.x);
    return GetTextStartX() + camera.x + fontRenderer->TextWidth(fontParameters.fontName, leftText);
}

glm::vec2 TEXT_PANEL::GetPixelShiftBorder()
{
    return glm::vec2(GetTextStartX() + 2*fontParameters.fontSize, dim.x - 2*fontParameters.fontSize);
}

// TODO, finish camera movement code, fix rightside clipping

void TEXT_PANEL::Render()
{
    int lineNumber = 0;

    int offsetLnNumbers = (glm::floor(log10(lines.size())) + 1) * fontParameters.fontSize;

    int totalLineHeight = fontParameters.GetLineHeight();

    for(auto it = lines.begin() + GetScrollBoundsVertical().x; it != (lines.end() < lines.begin() + GetScrollBoundsVertical().y ? lines.end() : lines.begin() + GetScrollBoundsVertical().y); it++,lineNumber++)
    {   
        std::string originalLine = *it;

        int textY = pos.y + fontParameters.fontPadding + (lineNumber * totalLineHeight);
        int textX = GetTextStartX() + camera.x;

        // line numbers
        int offsetLineNumber = lineNumber + GetScrollBoundsVertical().x;
        std::string lineString = std::to_string(offsetLineNumber + 1);
        fontRenderer->RenderText(fontParameters.fontName, lineString, pos.x + offsetLnNumbers - fontRenderer->TextWidth(fontParameters.fontName, lineString), textY, COLOR(0xFFFFFF));
         
        // line
        fontRenderer->RenderText(fontParameters.fontName, *it, textX, textY, COLOR(0xFFFFFF), glm::vec2(GetTextStartX(), dim.x));

        if(offsetLineNumber == cursor.y)
        {
            std::string leftText = (*it).substr(0, cursor.x - scrollOffset.x);

            int cursorRenderX = textX + fontRenderer->TextWidth(fontParameters.fontName, leftText);
            int cursorRenderY = textY - fontParameters.fontPadding;
            fontRenderer->RenderCursor(cursorRenderX, cursorRenderY, fontParameters.fontSize/8.0, totalLineHeight, COLOR(0xFFFFFF));
        }
    }
}

void TEXT_PANEL::MoveCursorUp()
{
    if(cursor.y >= 1)
    {
        // Move cursor to the end of the line above
        cursor.y = cursor.y - 1;
        cursor.x = glm::min<int>(cursor.x, lines.at(cursor.y).size());
        
        if(cursor.y < GetScrollBoundsVertical().x)
        {
            ScrollBoundsUp();
        }
    }
    else
    {
        // Move cursor to the start if you're at the top already
        cursor.y = 0;
        cursor.x = 0;
    }
}

void TEXT_PANEL::MoveCursorDown(){
    if(cursor.y < lines.size() - 1)
    {
        // Move the cursor down to the end of the next line, if it exists
        cursor.y = cursor.y + 1;
        cursor.x = glm::min<int>(cursor.x, lines.at(cursor.y).size());
        if(cursor.y >= GetScrollBoundsVertical().y)
        {
            ScrollBoundsDown();
        }
    }
}

void TEXT_PANEL::MoveCursorLeft(){
    if(cursor.x >= 1)
    {
        if(GetCursorPixelX() <= GetPixelShiftBorder().x && camera.x <= 0)
        {
            camera.x += fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(cursor.x - 1, 1));
            if(camera.x > 0)
                camera.x = 0;
        }
        // Move cursor left if its not at the start of a line
        cursor.x = cursor.x - 1;
    }
    else
    {
        // The cursor is at the start of a line
        if(cursor.y > 0)
        {
            // As long as this isn't the first line, move it to the end of
            // the line above.
            cursor.x = lines.at(cursor.y - 1).size();
            cursor.y -= 1;
            float textWidth = fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y));
            if(textWidth < dim.x)
            {
                // reset camera to the start if the line text-width is smaller than the textpanel.
                camera.x = 0;
            }
            else
            {
                // reset the camera so the last character is on the far-right side of the textpanel.
                // camera.x = panel width - text starting position - text width - 1x the font size
                camera.x = dim.x - GetTextStartX() - textWidth - fontParameters.fontSize;
            }
            if(cursor.y < GetScrollBoundsVertical().x)
            {
                ScrollBoundsUp();
            }
        }
    }
}
void TEXT_PANEL::MoveCursorRight(){
    if(cursor.x < lines.at(cursor.y).size())
    {
        // Move cursor to the right if it isn't at the end of a line
        cursor.x = cursor.x + 1;
        float textWidth = fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y));
        if(GetCursorPixelX() >= GetPixelShiftBorder().y && camera.x > dim.x - GetTextStartX() - textWidth)
        {
            camera.x -= fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(cursor.x, 1));
        }
    }
    else
    {
        // The cursor is at the end of a line
        if(cursor.y < lines.size() - 1)
        {
            // Move the cursor to the beginning of the next line aslong
            // as the line exists.
            cursor.x = 0;
            cursor.y += 1;
            camera.x = 0;
            if(cursor.y >= GetScrollBoundsVertical().y)
            {
                ScrollBoundsDown();
            }
        }
    }
}

void TEXT_PANEL::ScrollBoundsDown()
{
    scrollOffset.y += 1;
}

void TEXT_PANEL::ScrollBoundsUp()
{
    scrollOffset.y -= 1;
}



}