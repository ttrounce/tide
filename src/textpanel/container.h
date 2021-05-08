#ifndef TEXTPANEL_CONTAINER_H
#define TEXTPANEL_CONTAINER_H

#include "../engine/types.h"
#include "textpanel.h"
#include <fmt/core.h>

#define PANEL_TYPE_NONE 0
#define PANEL_TYPE_VERT 1
#define PANEL_TYPE_HORZ 2

struct PanelAncestor;

struct PanelNode
{
    PanelNode(PanelAncestor* ancestor, Optional<PanelNode*> parent, Unique<TextPanel> panel);
    
    void Batch();
    void TakeInput(InputType inputType, uint codepoint);
    void SplitVert();
    void SplitHorz();
    /// Internal function, please use PanelAncestor.SetFocusedNode
    void SetFocusInternal(bool flag);
    void SetRecursiveFrameSize(int x, int y, int width, int height);
    
    PanelAncestor* ancestor;
    Optional<PanelNode*> parent;

    int panelType{PANEL_TYPE_NONE};
    Unique<PanelNode> childA;
    Unique<PanelNode> childB;
    Unique<TextPanel> panel;
    bool isParent;
};

struct PanelAncestor
{
    void SetFocusedNode(PanelNode* focusedNode);
    void SplitFocusedVert();
    void SplitFocusedHorz();
    void UpdateFrameSize(int width, int height);
    void CycleFocus();
    void CloseFocusedNode();

    PanelAncestor(const Rect& rect, const TextPanelParameters& textPanelParameters);
    Unique<PanelNode> startNode;
    Optional<PanelNode*> focusedNode;
};

#endif // TEXTPANEL_CONTAINER_H