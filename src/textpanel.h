#ifndef TIDE_TEXT_PANEL_H
#define TIDE_TEXT_PANEL_H

#include "engine/types.h"
#include <string>
#include <vector>
#include <glm/vec2.hpp>

namespace tide
{

class TEXT_PANEL
{
private:
    std::string content;
    std::vector<std::string> lines;
public:
    struct {
        std::string fontName;
        int fontSize;
        int fontPadding;
        int GetLineHeight()
        {
            return (2*fontPadding + fontSize);
        }
    } fontParameters;
    
    glm::ivec2 scrollOffset;

    glm::vec2 cursor;
    glm::vec2 pos;
    glm::vec2 dim;

    TEXT_PANEL(glm::vec2 pos, glm::vec2 dim);
    
    bool isFocused;
    
    glm::ivec2 GetVerticalScrollBounds();
    glm::ivec2 GetHorizontalScrollBounds();

    void WriteChar(uint codepoint);
    void RemoveBackwardChar();
    void RemoveForwardChar();
    void NewLine();
    void Render();

    void MoveCursorUp();
    void MoveCursorDown();
    void MoveCursorLeft();
    void MoveCursorRight();

    void ScrollBoundsUp();
    void ScrollBoundsDown();
};

}

#endif // TIDE_TEXT_PANEL_H