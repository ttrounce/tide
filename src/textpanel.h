#ifndef TIDE_TEXT_PANEL_H
#define TIDE_TEXT_PANEL_H

#include "engine/types.h"
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace tide
{

enum INPUT_TYPE
{
    INPUT_WRITE,

    INPUT_REMOVE_BACK,
    INPUT_REMOVE_FWRD,

    INPUT_NEW_LINE,

    INPUT_MOVE_LEFT,
    INPUT_MOVE_RIGHT,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,

    INPUT_SCROLL_UP,
    INPUT_SCROLL_DOWN,

    INPUT_SHIFT_LEFT,
    INPUT_SHIFT_RIGHT
};

class TEXT_PANEL
{
private:
    std::vector<std::string> lines;
    
    glm::ivec2 GetCursorPixelPosition();
    glm::ivec2 GetPixelShiftBorder();
    BOUNDS GetContentBounds();

    void WriteChar(uint codepoint);
    int RemoveBackwardChar();
    void RemoveForwardChar();
    void NewLine();

    void MoveCursorUp();
    void MoveCursorDown();
    void MoveCursorLeft(int cursorX);
    void MoveCursorRight();
public:
    struct {
        std::string fontName;
        int fontSize;
        int fontPadding;
        int GetLineHeight() const
        {
            return (2*this->fontPadding + this->fontSize);
        }
    } fontParameters;
    
    glm::ivec2 camera;
    glm::ivec2 cursor;

    RECT panelRectangle;

    bool isFocused;

    TEXT_PANEL(RECT rect);

    void Render();
    void TakeInput(INPUT_TYPE type, uint codepoint);
};

}

#endif // TIDE_TEXT_PANEL_H