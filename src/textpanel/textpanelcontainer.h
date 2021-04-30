#ifndef H_TEXTPANEL_CONTAINER_H
#define H_TEXTPANEL_CONTAINER_H

#include "../engine/types.h"
#include "textpanel.h"

#include <unordered_map>

enum PanelContainerType
{
    NONE,
    VERTICAL,
    HORIZONTAL
};

class TextPanelContainer
{
private:
    void CollectChildren(std::vector<TextPanelContainer*>& collection);
    Shared<TextPanelContainer> firstContainer;
    Shared<TextPanelContainer> secondContainer;
public:
    Unique<TextPanel> panel;
    PanelContainerType containerType;

    TextPanelContainer* GetFirstChild();
    TextPanelContainer* GetSecondChild();

    TextPanelContainer(Unique<TextPanel> panel);

    void UpdateFrameSize(const Rect& newFrameSize);
    void SplitVertical();
    void SplitHorizontal();
    void SwitchWindowFocus();
    void TakeInput(InputType type, uint codepoint);

    void Batch();
};

#endif // H_TEXTPANEL_CONTAINER_H