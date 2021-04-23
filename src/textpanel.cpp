#include "textpanel.h"
#include "engine/gfx/font.h"

#include <fmt/core.h>

namespace tide
{
    
    TEXT_PANEL::TEXT_PANEL(RECT rect) : panelRectangle(rect)
    {
        cursor = {0, 0};
        camera = {0, 0};
        fontParameters = {"hack", 16, 2};
        lines.push_back(std::string());
    }

    BOUNDS TEXT_PANEL::GetContentBounds()
    {
        int x1 = panelRectangle.x + (glm::floor(log10(lines.size())) + 1) * fontParameters.fontSize + (fontParameters.fontSize/2.0);
        int y1 = 0;
        int x2 = panelRectangle.x + panelRectangle.w;
        int y2 = panelRectangle.y + panelRectangle.h - fontParameters.GetLineHeight();
        return BOUNDS(x1, y1, x2, y2);
    }

    glm::ivec2 TEXT_PANEL::GetCursorPixelPosition()
    {
        std::string leftText = lines.at(cursor.y).substr(0, cursor.x);
        int x = GetContentBounds().x1 + camera.x + fontRenderer->TextWidth(fontParameters.fontName, leftText);
        int y = GetContentBounds().y1 + camera.y + cursor.y * fontParameters.GetLineHeight();
        return glm::ivec2(x, y);
    }
    
    glm::ivec2 TEXT_PANEL::GetPixelShiftBorder()
    {
        return glm::vec2(GetContentBounds().x1 + 2*fontParameters.fontSize, GetContentBounds().x2 - 2*fontParameters.fontSize);
    }
    
    void TEXT_PANEL::TakeInput(INPUT_TYPE type, uint codepoint)
    {
        if(!isFocused)
            return;
        // used to pipeline all text-panel input interaction
        int removeBackPos = -1;
        switch(type)
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
                
        // after any type of input, we reposition the camera so that the cursor
        // is either as left as possible or as right as possible.
        if(GetCursorPixelPosition().x >= GetPixelShiftBorder().y)
        {
            camera.x = -fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(0, cursor.x)) + (GetContentBounds().x2 - GetContentBounds().x1) - 2*fontParameters.fontSize;
        } else if(cursor.x > 3 && GetCursorPixelPosition().x < GetPixelShiftBorder().x)
        {
            camera.x = -fontRenderer->TextWidth(fontParameters.fontName, lines.at(cursor.y).substr(0, cursor.x)) + 2*fontParameters.fontSize;
        } else if(cursor.x <= 3)
        {
            camera.x = 0;
        }
        
        if(GetCursorPixelPosition().y < GetContentBounds().y1)
        {
            camera.y = -fontParameters.GetLineHeight() * cursor.y;
        }
        
        if(GetCursorPixelPosition().y > GetContentBounds().y2 || GetCursorPixelPosition().y + fontParameters.GetLineHeight() >= GetContentBounds().y2)
        {
            camera.y = -fontParameters.GetLineHeight() * cursor.y + (GetContentBounds().y2 - GetContentBounds().y1) - fontParameters.GetLineHeight();
        }   
    }
    
    
    void TEXT_PANEL::WriteChar(uint codepoint)
    {
        lines.at(cursor.y).insert(cursor.x, 1, codepoint);
    }
    
    void TEXT_PANEL::RemoveForwardChar()
    {
        std::string line = lines.at(cursor.y);
        if(cursor.x < static_cast<int>(line.length()))
            lines.at(cursor.y) = line.substr(0, cursor.x) + line.substr(cursor.x + 1, line.length());
        else if(cursor.y < static_cast<int>(lines.size()) - 1)
        {
            lines.at(cursor.y) += lines.at(cursor.y + 1);
            lines.erase(lines.begin() + cursor.y + 1);
        }
    }
    
    int TEXT_PANEL::RemoveBackwardChar()
    {
        std::string line = lines.at(cursor.y);
        if(cursor.x > 0)
            lines.at(cursor.y) = line.substr(0, cursor.x - 1) + line.substr(cursor.x, line.length());
        else if(cursor.y > 0)
        {        
            int originalSize = lines.at(cursor.y - 1).length();
            lines.at(cursor.y - 1) = lines.at(cursor.y - 1) + line;
            lines.erase(lines.begin() + cursor.y);
            return originalSize;
        }
        return -1;
    }
    
    void TEXT_PANEL::NewLine()
    {
        std::string line = lines.at(cursor.y);
        if(cursor.x == static_cast<int>(line.length()))
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
    
    void TEXT_PANEL::MoveCursorUp()
    {
        if(cursor.y > 0)
            cursor.y -= 1;
        cursor.x = glm::min<int>(lines.at(cursor.y).length(), cursor.x);
    }
    
    void TEXT_PANEL::MoveCursorDown()
    {
        if(cursor.y < static_cast<int>(lines.size()) - 1)
            cursor.y += 1;
        cursor.x = glm::min<int>(lines.at(cursor.y).length(), cursor.x);
    }
    
    void TEXT_PANEL::MoveCursorLeft(int cursorX)
    {
        if(cursor.x > 0)
            cursor.x -= 1;
        else if(cursor.y > 0)
        {
            cursor.x = cursorX >= 0 ? cursorX : lines.at(cursor.y - 1).length();
            cursor.y -= 1;
        }
    }
    
    void TEXT_PANEL::MoveCursorRight()
    {
        if(cursor.x < static_cast<int>(lines.at(cursor.y).length()))
            cursor.x += 1;
        else if(cursor.y < static_cast<int>(lines.size()) - 1)
        {
            cursor.x = 0;
            cursor.y += 1;
        }
    }
    
    void TEXT_PANEL::Render()
    {
        int lineNumber = 0;
        
        int offsetLnNumbers = (glm::floor(log10(lines.size())) + 1) * fontParameters.fontSize;
        
        int totalLineHeight = fontParameters.GetLineHeight();
        
        for(auto it = lines.begin(); it != lines.end(); it++, lineNumber++)
        {     
            std::string& originalLine = *it;
            
            int lineY = GetContentBounds().y1 + (lineNumber * totalLineHeight) + camera.y;
            int textY = fontParameters.fontPadding + lineY;
            int textX = GetContentBounds().x1 + camera.x;

            // cull lines outside of content bounds
            if(lineY < GetContentBounds().y1 || lineY >= GetContentBounds().y2 || lineY + fontParameters.GetLineHeight() > GetContentBounds().y2)
                continue;
            
            // Render panel header
            fontRenderer->RenderCursor(0, GetContentBounds().y2, panelRectangle.w, fontParameters.GetLineHeight(), 1.0, COLOR(0xFFFFFF));
            fontRenderer->RenderText(fontParameters.fontName, "textpanel.cpp", 0, GetContentBounds().y2 + fontParameters.fontPadding, 2, COLOR(0x000000));

            // Render the base line-selector first for transparency. 
            std::string lineString = fmt::format("{}", lineNumber + 1);
            if(lineNumber == cursor.y)
            {
                fontRenderer->RenderCursor(GetContentBounds().x1, textY - fontParameters.fontPadding, panelRectangle.w, totalLineHeight, 1, COLOR(0x141414));
                // Render the line numbers
                fontRenderer->RenderText(fontParameters.fontName, lineString, panelRectangle.x + offsetLnNumbers - fontRenderer->TextWidth(fontParameters.fontName, lineString), textY, 2, COLOR(0xFFFFFF));
            }
            else
            {
                fontRenderer->RenderText(fontParameters.fontName, lineString, panelRectangle.x + offsetLnNumbers - fontRenderer->TextWidth(fontParameters.fontName, lineString), textY, 2, COLOR(0x444444));
            }
            
            // Render the line text
            fontRenderer->RenderText(fontParameters.fontName, originalLine, textX, textY, 2, COLOR(0xFFFFFF), glm::vec2(GetContentBounds().x1, GetContentBounds().x2));
            
            // Render the cursor on top of everything.
            if(lineNumber == cursor.y)
            {
                std::string leftText = (originalLine).substr(0, cursor.x);
                
                int cursorRenderX = textX + fontRenderer->TextWidth(fontParameters.fontName, leftText);
                int cursorRenderY = textY - fontParameters.fontPadding;
                fontRenderer->RenderCursor(cursorRenderX, cursorRenderY, fontParameters.fontSize/8.0, totalLineHeight, 3, COLOR(0xFFFFFF));
            }
        }
    }
    
}