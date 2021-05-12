#ifndef TIDE_TEXT_PANEL_H
#define TIDE_TEXT_PANEL_H

#include "../../engine/types.h"
#include "../../engine/gfx/font.h"
#include "../fontparams.h"
#include "../component.h"
#include <string>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace textpanel
{
enum InputType
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
}

class TextPanel
{
private:
    std::vector<std::string> lines;

    glm::ivec2 GetCursorPixelPosition();
    glm::ivec2 GetPixelShiftBorder();
    Bounds GetContentBounds();

    void WriteChar(uint codepoint);
    int RemoveBackwardChar();
    void RemoveForwardChar();
    void NewLine();

    void MoveCursorUp();
    void MoveCursorDown();
    void MoveCursorLeft(int cursorX);
    void MoveCursorRight();
public:

    Rect panelRectangle;

    // A wrapper around the text panel parameters, with some checking to avoid UB.
    const FontParams panelParameters;

    glm::ivec2 camera;
    glm::ivec2 cursor;

    bool isFocused;

    TextPanel(const Rect& rect, const FontParams& params);
    ~TextPanel(){}

    void Batch();
    void TakeInput(int inputType, uint codepoint);
};


#endif // TIDE_TEXT_PANEL_H