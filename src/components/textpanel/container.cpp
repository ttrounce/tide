#include "container.h"
#include "../../engine/assert.h"

#include <fmt/printf.h>
#include <fmt/core.h>
#include <utility>
#include <vector>

// Panel Ancestor #################

void PanelAncestor::SetFocusedNode(PanelNode* node)
{
    if (focusedNode.has_value())
    {
        focusedNode.value()->SetFocusInternal(false);
    }
    this->focusedNode = std::make_optional(node);
    focusedNode.value()->SetFocusInternal(true);
}

void PanelAncestor::SplitFocusedVert()
{
    if (focusedNode.has_value())
    {
        focusedNode.value()->SplitVert();
    }
}

void PanelAncestor::SplitFocusedHorz()
{
    if (focusedNode.has_value())
    {
        focusedNode.value()->SplitHorz();
    }
}

void PanelAncestor::UpdateFrameSize(int width, int height)
{
    TIDE_ASSERT(startNode);

    if (startNode)
    {
        startNode->SetRecursiveFrameSize(0, 0, width, height);
    }
}

void CollectNodes(std::vector<PanelNode*>& collection, PanelNode* node)
{
    if (node->IsParent())
    {
        CollectNodes(collection, node->GetChildA().value());
        CollectNodes(collection, node->GetChildB().value());
    }
    else
    {
        collection.push_back(node);
    }
}

void PanelAncestor::CycleFocus()
{
    if (startNode->isParent)
    {
        std::vector<PanelNode*> collection;
        CollectNodes(collection, startNode.get());

        int index = 0;
        for (auto*& node : collection)
        {
            if (!node->isParent && node->panel->isFocused)
            {
                SetFocusedNode(collection.at((index + 1) % collection.size()));
                break;
            }
            index++;
        }
    }
}

void PanelAncestor::CloseFocusedNode()
{
    if (focusedNode.has_value())
    {
        auto*& focused = focusedNode.value();
        if (focused->parent.has_value())
        {
            auto*& parent = focused->parent.value();
            if (parent->parent.has_value())
            {
                auto*& grandparent = parent->parent.value();
                if (!focused->isParent)
                {
                    Unique<PanelNode> intermediary = nullptr;
                    if (focused == parent->childB.get())
                    {
                        CycleFocus();
                        intermediary = std::move(parent->childA);
                    }
                    else if (focused == parent->childA.get())
                    {
                        CycleFocus();
                        intermediary = std::move(parent->childB);
                    }
                    else
                    {
                        TIDE_ABORT("The focused node SHOULD be a child of the parent");
                    }

                    if (parent == grandparent->childA.get())
                    {
                        grandparent->childA = std::move(intermediary);
                        grandparent->childA->parent = std::optional(grandparent);
                    }
                    else if (parent == grandparent->childB.get())
                    {
                        grandparent->childB = std::move(intermediary);
                        grandparent->childB->parent = std::optional(grandparent);
                    }
                    else
                    {
                        TIDE_ABORT("The parent node SHOULD be a child of the grandparent");
                    }
                }
                else
                {
                    TIDE_ABORT("Erroneous state");
                }
            }
            else
            {
                if (!focused->isParent)
                {
                    if (focused == parent->childB.get())
                    {
                        CycleFocus();
                        this->startNode = std::move(parent->childA);
                    }
                    else if (focused == parent->childA.get())
                    {
                        CycleFocus();
                        this->startNode = std::move(parent->childB);
                    }
                    else
                    {
                        TIDE_ABORT("The focused node SHOULD be a child of the parent");
                    }
                    this->startNode->parent.reset();
                }
                else
                {
                    TIDE_ABORT("Erroneous state");
                }
            }
        }
    }
}

void PanelAncestor::Batch()
{
    if(startNode)
    {
        startNode->Batch();
    }
}

void PanelAncestor::TakeInput(int inputType, uint codepoint)
{
    if(startNode)
    {
        startNode->TakeInput(inputType, codepoint);
    }
}

PanelAncestor::PanelAncestor(const Rect& rect, const FontParams& textPanelParameters)
{
    this->startNode = std::make_unique<PanelNode>(this, std::optional<PanelNode*>{}, std::make_unique<TextPanel>(rect, textPanelParameters));
}

// Panel Node #####################

PanelNode::PanelNode(PanelAncestor* ancestor, std::optional<PanelNode*> p, Unique<TextPanel> panel)
    : ancestor(ancestor), parent(p)
{
    this->isParent = false;
    this->panel = std::move(panel);
}

bool PanelNode::IsParent() const
{
    return isParent;
}

Optional<PanelNode*> PanelNode::GetChildA()
{
    if(childA)
    {
        return childA.get();
    }
    return Optional<PanelNode*>();
}

Optional<PanelNode*> PanelNode::GetChildB()
{
    if(childB)
    {
        return childB.get();
    }
    return Optional<PanelNode*>();
}

Optional<TextPanel*> PanelNode::GetPanel()
{
    if(panel)
    {
        return panel.get();
    }
    return Optional<TextPanel*>();
}

void PanelNode::SetFocusInternal(bool flag)
{
    if (panel)
    {
        panel->isFocused = flag;
    }
}

void PanelNode::SetRecursiveFrameSize(int x, int y, int width, int height)
{
    if (this->panel)
    {
        this->panel->panelRectangle = Rect{ x, y, width, height };
    }
    else
    {
        switch (panelType)
        {
        case PANEL_TYPE_NONE:
        {
            TIDE_ABORT("Erroneous State");
            break;
        }
        case PANEL_TYPE_HORZ:
        {
            this->childA->SetRecursiveFrameSize(x, y, width, height / 2);
            this->childB->SetRecursiveFrameSize(x, y + height / 2, width, height / 2);
            break;
        }
        case PANEL_TYPE_VERT:
        {
            this->childA->SetRecursiveFrameSize(x, y, width / 2, height);
            this->childB->SetRecursiveFrameSize(x + width / 2, y, width / 2, height);
            break;
        }
        }
    }
}

void PanelNode::TakeInput(int inputType, uint codepoint)
{
    if (panel)
    {
        panel->TakeInput(inputType, codepoint);
    }
    else
    {
        if(childA) childA->TakeInput(inputType, codepoint);
        if(childB) childB->TakeInput(inputType, codepoint);
    }
}

void PanelNode::SplitVert()
{
    if (!isParent)
    {
        TIDE_ASSERT(panel);
        TIDE_ASSERT(ancestor);


        const auto& rectO = this->panel->panelRectangle;
        if (rectO.w <= 1 || rectO.h <= 1)
        {
            return;
        }
        auto rectA = Rect{ rectO.x, rectO.y, rectO.w / 2, rectO.h };
        auto rectB = Rect{ rectO.x + rectO.w / 2, rectO.y, rectO.w / 2, rectO.h };

        this->panel->panelRectangle = rectA;
        auto copiedParameters = panel->panelParameters;

        childA = std::make_unique<PanelNode>(this->ancestor, std::make_optional(this), std::move(this->panel));
        childB = std::make_unique<PanelNode>(this->ancestor, std::make_optional(this), std::make_unique<TextPanel>(rectB, copiedParameters));
        isParent = true;

        panelType = PANEL_TYPE_VERT;

        if (childA->panel->isFocused)
        {
            ancestor->SetFocusedNode(childA.get());
        }
    }
}

void PanelNode::SplitHorz()
{
    if (!isParent)
    {
        TIDE_ASSERT(panel);
        TIDE_ASSERT(ancestor);

        const auto& rectO = this->panel->panelRectangle;
        if (rectO.w <= 1 || rectO.h <= 1)
        {
            // TODO: add some sort on GUI error "too many panels, etc"
            return;
        }
        auto rectA = Rect{ rectO.x, rectO.y, rectO.w, rectO.h / 2 };
        auto rectB = Rect{ rectO.x, rectO.y + rectO.h / 2, rectO.w, rectO.h / 2 };

        this->panel->panelRectangle = rectA;
        auto copiedParameters = panel->panelParameters;

        childA = std::make_unique<PanelNode>(this->ancestor, std::make_optional(this), std::move(this->panel));
        childB = std::make_unique<PanelNode>(this->ancestor, std::make_optional(this), std::make_unique<TextPanel>(rectB, copiedParameters));
        isParent = true;

        panelType = PANEL_TYPE_HORZ;

        if (childA->panel->isFocused)
        {
            ancestor->SetFocusedNode(childA.get());
        }
    }
}

void PanelNode::Batch()
{
    if (isParent)
    {
        TIDE_ASSERT(childA && childB);
        childA->Batch();
        childB->Batch();
    }
    else
    {
        TIDE_ASSERT(panel);

        panel->Batch();
    }
}

namespace container
{
    
void PrintIndentedString(std::string str, int depth, std::vector<bool> indents, bool endNode)
{
    for (int i = 0; i < depth; i++)
    {
        bool b = indents[i];
        if (b)
        {
            fmt::printf("%c    ", (char)179);
        }
        else
        {
            fmt::print("     ");
        }
    }
    fmt::printf("%c%c %s\n", (char)endNode ? 192 : 195, (char)240, str);
}

void PrintPanelSubnode(PanelNode* node, int depth, std::vector<bool> indents)
{

    if (node)
    {
        auto optionalChildA = node->GetChildA();
        auto optionalChildB = node->GetChildB();
        if (optionalChildA.has_value())
        {
            auto v = std::vector(depth, true);
            if (depth > 0)
            {
                v[glm::max(0, depth - 1)] = true;
            }
            if (depth >= static_cast<int>(indents.size()))
                indents.push_back(true);
            else
                indents[depth] = true;
            if (optionalChildA.value()->IsParent())
            {
                PrintIndentedString(fmt::format("childA: {} (PARENT)", (void*) optionalChildA.value()), depth, indents, false);
            }
            else
            {
                PrintIndentedString(fmt::format("childA: {} ({})", (void*) optionalChildA.value(), (void*) optionalChildA.value()->GetPanel().value_or(nullptr)), depth, indents, false);
            }
            PrintPanelSubnode(optionalChildA.value(), depth + 1, indents);
        }
        else
        {
            PrintIndentedString(fmt::format("childA: null"), depth, indents, false);
        }
        if (optionalChildB.has_value())
        {
            if (depth >= static_cast<int>(indents.size()))
                indents.push_back(false);
            else
                indents[depth] = false;
            if (optionalChildB.value()->IsParent())
            {
                PrintIndentedString(fmt::format("childB: {} (PARENT)", (void*) optionalChildB.value()), depth, indents, true);
            }
            else
            {
                PrintIndentedString(fmt::format("childB: {} ({})", (void*) optionalChildB.value(), (void*)optionalChildB.value()), depth, indents, true);
            }
            PrintPanelSubnode(optionalChildB.value(), depth + 1, indents);
        }
        else
        {
            PrintIndentedString(fmt::format("childB: null"), depth, indents, true);
        }
    }
}

void PrintPanelTree(PanelAncestor* ancestor)
{
    fmt::printf("%c\n", (char)220);
    PrintPanelSubnode(ancestor->startNode.get(), 0, {});
    fmt::print("\n");
}

}