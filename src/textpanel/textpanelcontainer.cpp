#include "textpanelcontainer.h"
#include <fmt/core.h>
#include <utility>
#include <vector>

TextPanelContainer::TextPanelContainer(Unique<TextPanel> panel) : panel(std::move(panel)), containerType(NONE)
{}

void TextPanelContainer::CollectChildren(std::vector<TextPanelContainer*>& collection)
{
    if(firstContainer->containerType == NONE)
    {
        collection.push_back(firstContainer.get());
    }
    else
    {
        firstContainer->CollectChildren(collection);
    }

    if(secondContainer->containerType == NONE)
    {
        collection.push_back(secondContainer.get());
    }
    else
    {
        secondContainer->CollectChildren(collection);
    }
}

void TextPanelContainer::UpdateFrameSize(const Rect& newFrameSize)
{
    if(this->containerType == NONE)
    {
        this->panel->panelRectangle = newFrameSize;
        return;
    }
    else if(this->containerType == VERTICAL)
    {
        firstContainer->UpdateFrameSize({newFrameSize.x, newFrameSize.y, newFrameSize.w / 2, newFrameSize.h});
        secondContainer->UpdateFrameSize({newFrameSize.x + newFrameSize.w / 2, newFrameSize.y, newFrameSize.w / 2, newFrameSize.h});
    }
    else
    {
        firstContainer->UpdateFrameSize({newFrameSize.x, newFrameSize.y, newFrameSize.w, newFrameSize.h / 2});
        secondContainer->UpdateFrameSize({newFrameSize.x, newFrameSize.y + newFrameSize.h / 2, newFrameSize.w, newFrameSize.h / 2});
    }
}

void TextPanelContainer::SplitVertical()
{
    if (panel)
    {
        this->containerType = VERTICAL;

        Rect rectMain = panel->panelRectangle;
        Rect rectOther = Rect(0, 0, 0, 0);
        
        this->panel->panelRectangle = { rectMain.x, rectMain.y, rectMain.w / 2, rectMain.h };
        rectOther = { rectMain.x + rectMain.w / 2, rectMain.y, rectMain.w / 2, rectMain.h };

        this->firstContainer = std::make_unique<TextPanelContainer>(std::move(this->panel));
        this->secondContainer = std::make_unique<TextPanelContainer>(std::make_unique<TextPanel>(rectOther));
        
        this->panel = nullptr;
    }
}
void TextPanelContainer::SplitHorizontal()
{
    if (panel)
    {
        this->containerType = HORIZONTAL;

        Rect rectMain = panel->panelRectangle;
        Rect rectOther = Rect(0, 0, 0, 0);

        this->panel->panelRectangle = { rectMain.x, rectMain.y, rectMain.w, rectMain.h / 2 };
        rectOther = { rectMain.x, rectMain.y + rectMain.h / 2, rectMain.w, rectMain.h / 2 };

        this->firstContainer = std::make_shared<TextPanelContainer>(std::move(this->panel));
        this->secondContainer = std::make_shared<TextPanelContainer>(std::make_unique<TextPanel>(rectOther));

        this->panel = nullptr;
    }
}

void TextPanelContainer::SwitchWindowFocus()
{
    if(this->containerType == NONE)
        return;

    std::vector<TextPanelContainer*> tempContainers;
    this->CollectChildren(tempContainers);

    int index = 0;
    for(auto i = tempContainers.begin(); i != tempContainers.end(); i++, index++)
    {
        if((*i)->panel->isFocused)
        {
            tempContainers.at(index)->panel->isFocused = false;
            tempContainers.at((index + 1) % tempContainers.size())->panel->isFocused = true;
            break;
        }
    }

}

void TextPanelContainer::TakeInput(InputType type, uint codepoint)
{
    if (panel)
    {
        panel->TakeInput(type, codepoint);
    }
    else
    {
        firstContainer->TakeInput(type, codepoint);
        secondContainer->TakeInput(type, codepoint);
    }
}

void TextPanelContainer::RenderAll()
{
    if (panel)
    {
        panel->Render();
    }
    else
    {
        firstContainer->RenderAll();
        secondContainer->RenderAll();
    }
}