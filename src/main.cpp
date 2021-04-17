#define TIDE_ENGINE_IMPLEMENTATION
#include "engine/engine.h"

#include <iostream>
#include <pthread.h>

#include "engine/gfx/font.h"
#include "engine/gfx/gfx.h"
#include "textpanel.h"
#include "prefab/model.h"

void Update(double t, double dt)
{

}

tide::TEXT_PANEL* panel;

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    panel->Render();

    tide::fontRenderer->RenderText("hack", std::to_string(engine->window->fps), engine->GetWindowWidth()  - tide::fontRenderer->TextWidth("hack", std::to_string(engine->window->fps)), engine->GetFrameBufferHeight() - 32, COLOR(0xFFFFFF));
    
    std::string cursorString = std::to_string((int)panel->cursor.x) + ":" + std::to_string((int)panel->cursor.y);
    tide::fontRenderer->RenderText("hack", cursorString, engine->GetWindowWidth() - tide::fontRenderer->TextWidth("hack", cursorString), engine->GetFrameBufferHeight() - 16, COLOR(0xFFFFFF));
}

void OnCharAction(uint codepoint)
{
    panel->WriteChar(codepoint);
    std::cout << panel->cursor.x << ", " << panel->cursor.y << std::endl;
}

void OnKeyAction(int key, int action, int scancode)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if(key == GLFW_KEY_BACKSPACE)
        {
            panel->RemoveBackwardChar();
        }
        if(key == GLFW_KEY_DELETE)
        {
            panel->RemoveForwardChar();
        }
        if(key == GLFW_KEY_TAB)
        {
            panel->WriteChar('\t');
        }
        if(key == GLFW_KEY_ENTER)
        {
            panel->NewLine();
        }
        if(key == GLFW_KEY_UP)
        {
            panel->MoveCursorUp();
        }
        if(key == GLFW_KEY_DOWN)
        {
            panel->MoveCursorDown();
        }
        if(key == GLFW_KEY_LEFT)
        {
            panel->MoveCursorLeft();
        }
        if(key == GLFW_KEY_RIGHT)
        {
            panel->MoveCursorRight();
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
        engine->resizeListeners.push_back([](int width, int height){
            panel->dim = {width, height};
        });

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        tide::InitFreeType();
        tide::InitialisePrefab();
        tide::fontRenderer->LoadFace("hack", "hack.ttf", 16);

        panel = new tide::TEXT_PANEL(glm::vec2(0, 0), glm::vec2(engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight()));
        panel->isFocused = true;
        engine->Start(Update, Draw);
    }
    else
    {
        std::cout << "[TIDE] " << "Engine failed to build" << std::endl;
    }
    delete panel;
    tide::FreeFreeType();
}