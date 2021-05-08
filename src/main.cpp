#define TIDE_ENGINE_IMPLEMENTATION
#include "engine/engine.h"

#include <fmt/core.h>
#include <pthread.h>

#include "engine/gfx/font.h"
#include "engine/gfx/gfx.h"
#include "textpanel/textpanel.h"
#include "prefab/model.h"
#include "textpanel/container.h"
#include "engine/types.h"

void Update(double t, double dt)
{

}

Unique<PanelAncestor> baseContainer;

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    baseContainer->startNode->Batch();
    std::string fpsText = fmt::format("{:.1f}ms {}fps", engine->window->frameTime, engine->window->fps);
    fontRenderer->BatchText("hack-10", fpsText, engine->GetWindowWidth() - fontRenderer->TextWidth("hack-10", fpsText) - 3, 5, 5.0, Color{ 0xFFFFFF });

    // std::string mouseText = fmt::format("{:.0f}:{:.0f}", engine->window->mouse->pos.x, engine->window->mouse->pos.y);
    // fontRenderer->RenderText("hack", mouseText, engine->GetFrameBufferWidth() - fontRenderer->TextWidth("hack", mouseText), panel->fontParameters.linePadding + panel->fontParameters.GetLineHeight(), 2, Color(0xFFFFFF));

    // std::string cursorString = fmt::format("{}:{}", panel->cursor.x, panel->cursor.y);
    // fontRenderer->RenderText("hack", cursorString, engine->GetWindowWidth() - fontRenderer->TextWidth("hack", cursorString), engine->GetFrameBufferHeight() - panel->fontParameters.GetLineHeight() + panel->fontParameters.linePadding, 2, Color(0x000000));

    fontRenderer->Render();
}

void OnCharAction(uint codepoint)
{
    auto& node = baseContainer->focusedNode;
    if (node.has_value())
    {
        node.value()->TakeInput(INPUT_WRITE, codepoint);
    }
}

#include <bitset>

void PrintPanelSubnode(PanelNode* node, int depth)
{
    if(node)
    {
        if(node->childA)
        {
            for(int i = 0; i < depth; i++)
            {
                fmt::print("\t");
            }
            fmt::print("|-> childA: {} ({})\n", (void*) node->childA.get(), (void*) node->childA->panel.get());
            PrintPanelSubnode(node->childA.get(), depth+1);
        }
        else
        {
            for(int i = 0; i < depth; i++)
            {
                fmt::print("\t");
            }
            fmt::print("|-> childA: null\n");
        }
        if(node->childB)
        {
            for(int i = 0; i < depth; i++)
            {
                fmt::print("\t");
            }
            fmt::print("|-> childB: {} ({})\n", (void*) node->childB.get(), (void*) node->childB->panel.get());
            PrintPanelSubnode(node->childB.get(), depth+1);
        }
        else
        {
            for(int i = 0; i < depth; i++)
            {
                fmt::print("\t");
            }
            fmt::print("|-> childB: null\n");
        }
    }
}

void PrintPanelTree()
{
    fmt::print("---ANCESTOR---\n");
    fmt::print(""); PrintPanelSubnode(baseContainer->startNode.get(), 0);
    fmt::print("--------------\n");
}

void OnKeyAction(int key, int action, int scancode)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_F1)
        {
            baseContainer->CycleFocus();
        }
        if(key == GLFW_KEY_F2)
        {
            baseContainer->SplitFocusedHorz();
        }
        if(key == GLFW_KEY_F3)
        {
            baseContainer->SplitFocusedVert();
        }
        if(key == GLFW_KEY_F4)
        {
            baseContainer->CloseFocusedNode();
            baseContainer->UpdateFrameSize(engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight());
        }
        if(key == GLFW_KEY_F5)
        {
            PrintPanelTree();
        }
        auto& node = baseContainer->focusedNode;
        if (node.has_value())
        {
            if (key == GLFW_KEY_BACKSPACE)
            {
                node.value()->TakeInput(INPUT_REMOVE_BACK, 0);
            }
            if (key == GLFW_KEY_DELETE)
            {
                node.value()->TakeInput(INPUT_REMOVE_FWRD, 0);
            }
            if (key == GLFW_KEY_TAB)
            {
                node.value()->TakeInput(INPUT_WRITE, '\t');
            }
            if (key == GLFW_KEY_ENTER)
            {
                node.value()->TakeInput(INPUT_NEW_LINE, 0);
            }
            if (key == GLFW_KEY_UP)
            {
                node.value()->TakeInput(INPUT_MOVE_UP, 0);
            }
            if (key == GLFW_KEY_DOWN)
            {
                node.value()->TakeInput(INPUT_MOVE_DOWN, 0);
            }
            if (key == GLFW_KEY_LEFT)
            {
                node.value()->TakeInput(INPUT_MOVE_LEFT, 0);
            }
            if (key == GLFW_KEY_RIGHT)
            {
                node.value()->TakeInput(INPUT_MOVE_RIGHT, 0);
            }
        }
    }
}
int main()
{
    engine = std::make_unique<Engine>(800, 600, "T.IDE");
    if (engine->GetStatus())
    {
        fmt::print("GL Version: {}\n", glGetString(GL_VERSION));

        engine->window->frameRateTarget = 60;
        engine->charListeners.push_back(OnCharAction);
        engine->keyListeners.push_back(OnKeyAction);
        engine->resizeFrameBufferListeners.push_back([](int width, int height) {
            fontRenderer->UpdateOrthographic(width, height);
            baseContainer->UpdateFrameSize(width, height);
            });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        InitFreeType();
        InitialisePrefab();
        fontRenderer->LoadFace("hack", "fonts/hack.ttf", 16);
        fontRenderer->CloneFace("hack", "hack-10", 10);

        baseContainer = std::make_unique<PanelAncestor>(Rect{ 0,0,800,600 }, TextPanelParameters{ "hack", 2 });
        baseContainer->SetFocusedNode(baseContainer->startNode.get());
        // baseContainer->SplitHorizontal();
        // baseContainer->GetSecondChild()->SplitVertical();

        engine->Start(Update, Draw);
    }
    else
    {
        fmt::print("[TIDE] Engine failed to build\n");
    }
    FreeFreeType();
}