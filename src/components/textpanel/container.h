#ifndef TEXTPANEL_CONTAINER_H
#define TEXTPANEL_CONTAINER_H

#include "../../engine/types.h"
#include "../component.h"
#include "textpanel.h"
#include <fmt/core.h>

#define PANEL_TYPE_NONE 0
#define PANEL_TYPE_VERT 1
#define PANEL_TYPE_HORZ 2

class PanelAncestor;

struct PanelNode
{
public:
    friend struct PanelAncestor;
    PanelNode(PanelAncestor* ancestor, Optional<PanelNode*> parent, Unique<TextPanel> panel);
    bool IsParent() const;
    Optional<PanelNode*> GetChildA();
    Optional<PanelNode*> GetChildB();
    Optional<TextPanel*> GetPanel();
private:
    void Batch();
    void TakeInput(int inputType, uint codepoint);
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

class PanelAncestor : public ActableComponent
{
public:
    void SetFocusedNode(PanelNode* focusedNode);
    void SplitFocusedVert();
    void SplitFocusedHorz();
    void UpdateFrameSize(int width, int height);
    void CycleFocus();
    void CloseFocusedNode();
    void Batch();
    void TakeInput(int inputType, uint codepoint);

    PanelAncestor(const Rect& rect, const FontParams& textPanelParameters);
    ~PanelAncestor() {}
    Unique<PanelNode> startNode;
    Optional<PanelNode*> focusedNode;
};

namespace container
{

void PrintIndentedString(std::string str, int depth, std::vector<bool> indents, bool endNode);
void PrintPanelSubnode(PanelNode* node, int depth, std::vector<bool> indents);
void PrintPanelTree(PanelAncestor* ancestor);

};

#endif // TEXTPANEL_CONTAINER_H