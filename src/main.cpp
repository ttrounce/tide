#define TIDE_ENGINE_IMPLEMENTATION
#include "engine/engine.h"

#include <fmt/core.h>
#include <pthread.h>

#include "engine/gfx/font.h"
#include "engine/gfx/gfx.h"
#include "textpanel.h"
#include "prefab/model.h"
#include "engine/types.h"

void Update(double t, double dt)
{

}

tide::TEXT_PANEL* panel;

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    panel->Render();

    std::string fpsText = fmt::format("{}fps", engine->window->fps);
    tide::fontRenderer->RenderText("hack", fpsText, engine->GetWindowWidth()  - tide::fontRenderer->TextWidth("hack", fpsText), panel->fontParameters.fontPadding, 2, COLOR(0xFFFFFF));
    
    std::string mouseText = fmt::format("{:.0f}:{:.0f}", engine->window->mouse->pos.x, engine->window->mouse->pos.y);
    tide::fontRenderer->RenderText("hack", mouseText, engine->GetFrameBufferWidth() - tide::fontRenderer->TextWidth("hack", mouseText), panel->fontParameters.fontPadding + panel->fontParameters.GetLineHeight(), 2, COLOR(0xFFFFFF));

    std::string cursorString = fmt::format("{}:{}", panel->cursor.x, panel->cursor.y);
    tide::fontRenderer->RenderText("hack", cursorString, engine->GetWindowWidth() - tide::fontRenderer->TextWidth("hack", cursorString), engine->GetFrameBufferHeight() - panel->fontParameters.GetLineHeight() + panel->fontParameters.fontPadding, 2, COLOR(0x000000));
}

void OnCharAction(uint codepoint)
{
    panel->TakeInput(tide::INPUT_WRITE, codepoint);
}

void OnKeyAction(int key, int action, int scancode)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if(key == GLFW_KEY_BACKSPACE)
        {
            panel->TakeInput(tide::INPUT_REMOVE_BACK, 0);
        }
        if(key == GLFW_KEY_DELETE)
        {
            panel->TakeInput(tide::INPUT_REMOVE_FWRD, 0);
        }
        if(key == GLFW_KEY_TAB)
        {
            panel->TakeInput(tide::INPUT_WRITE, '\t');
        }
        if(key == GLFW_KEY_ENTER)
        {
            panel->TakeInput(tide::INPUT_NEW_LINE, 0);
        }
        if(key == GLFW_KEY_UP)
        {
            panel->TakeInput(tide::INPUT_MOVE_UP, 0);
        }
        if(key == GLFW_KEY_DOWN)
        {
            panel->TakeInput(tide::INPUT_MOVE_DOWN, 0);
        }
        if(key == GLFW_KEY_LEFT)
        {
            panel->TakeInput(tide::INPUT_MOVE_LEFT, 0);
        }
        if(key == GLFW_KEY_RIGHT)
        {
            panel->TakeInput(tide::INPUT_MOVE_RIGHT, 0);
        }
    }
}

int main()
{   
    engine = std::make_unique<tide::ENGINE>(800, 600, "T.IDE");
    if(engine->GetStatus())
    {
        engine->charListeners.push_back(OnCharAction);
        engine->keyListeners.push_back(OnKeyAction);
        engine->resizeFrameBufferListeners.push_back([](int width, int height){
            panel->panelRectangle.w = width;
            panel->panelRectangle.h = height;
        });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        tide::InitFreeType();
        tide::InitialisePrefab();
        tide::fontRenderer->LoadFace("hack", "hack.ttf", 16);

        panel = new tide::TEXT_PANEL(RECT(0, 0, engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight()));
        panel->isFocused = true;
        engine->Start(Update, Draw);
    }
    else
    {
        fmt::print("[TIDE] Engine failed to build\n");
    }
    delete panel;
    tide::FreeFreeType();
}