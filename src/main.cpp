#define TIDE_ENGINE_IMPLEMENTATION
#include "engine/engine.h"

#include <fmt/core.h>
#include <pthread.h>

#include "engine/gfx/font.h"
#include "engine/gfx/gfx.h"
#include "textpanel/textpanel.h"
#include "prefab/model.h"
#include "textpanel/textpanelcontainer.h"
#include "engine/types.h"

void Update(double t, double dt)
{

}

Unique<TextPanelContainer> baseContainer;

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    baseContainer->RenderAll();

    std::string fpsText = fmt::format("{:.1f}us {}fps", engine->window->frameTime, engine->window->fps);
    fontRenderer->RenderText("hack", fpsText, engine->GetWindowWidth() - fontRenderer->TextWidth("hack", fpsText), 2, 2, Color{0xFFFFFF});

    // std::string mouseText = fmt::format("{:.0f}:{:.0f}", engine->window->mouse->pos.x, engine->window->mouse->pos.y);
    // fontRenderer->RenderText("hack", mouseText, engine->GetFrameBufferWidth() - fontRenderer->TextWidth("hack", mouseText), panel->fontParameters.linePadding + panel->fontParameters.GetLineHeight(), 2, Color(0xFFFFFF));

    // std::string cursorString = fmt::format("{}:{}", panel->cursor.x, panel->cursor.y);
    // fontRenderer->RenderText("hack", cursorString, engine->GetWindowWidth() - fontRenderer->TextWidth("hack", cursorString), engine->GetFrameBufferHeight() - panel->fontParameters.GetLineHeight() + panel->fontParameters.linePadding, 2, Color(0x000000));
}

void OnCharAction(uint codepoint)
{
    baseContainer->TakeInput(INPUT_WRITE, codepoint);
}

void OnKeyAction(int key, int action, int scancode)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if(key == GLFW_KEY_F1)
        {
            baseContainer->SwitchWindowFocus();
        }
        if (key == GLFW_KEY_BACKSPACE)
        {
            baseContainer->TakeInput(INPUT_REMOVE_BACK, 0);
        }
        if (key == GLFW_KEY_DELETE)
        {
            baseContainer->TakeInput(INPUT_REMOVE_FWRD, 0);
        }
        if (key == GLFW_KEY_TAB)
        {
            baseContainer->TakeInput(INPUT_WRITE, '\t');
        }
        if (key == GLFW_KEY_ENTER)
        {
            baseContainer->TakeInput(INPUT_NEW_LINE, 0);
        }
        if (key == GLFW_KEY_UP)
        {
            baseContainer->TakeInput(INPUT_MOVE_UP, 0);
        }
        if (key == GLFW_KEY_DOWN)
        {
            baseContainer->TakeInput(INPUT_MOVE_DOWN, 0);
        }
        if (key == GLFW_KEY_LEFT)
        {
            baseContainer->TakeInput(INPUT_MOVE_LEFT, 0);
        }
        if (key == GLFW_KEY_RIGHT)
        {
            baseContainer->TakeInput(INPUT_MOVE_RIGHT, 0);
        }
    }
}
int main()
{
    engine = std::make_unique<Engine>(800, 600, "T.IDE");
    if (engine->GetStatus())
    {
        engine->window->frameRateTarget = 10000;
        engine->charListeners.push_back(OnCharAction);
        engine->keyListeners.push_back(OnKeyAction);
        engine->resizeFrameBufferListeners.push_back([](int width, int height) {
            baseContainer->UpdateFrameSize({0, 0, width, height});
        });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        InitFreeType();
        InitialisePrefab();
        fontRenderer->LoadFace("hack", "hack.ttf", 16);

        Unique<TextPanel> panel = std::make_unique<TextPanel>(Rect{0, 0, 800, 600});
        panel->fontParameters = { "hack", 2 };
        panel->isFocused = true;

        baseContainer = std::make_unique<TextPanelContainer>(std::move(panel));
        baseContainer->SplitHorizontal();
        baseContainer->GetSecondChild()->SplitVertical();

        engine->Start(Update, Draw);
    }
    else
    {
        fmt::print("[TIDE] Engine failed to build\n");
    }

    FreeFreeType();
}