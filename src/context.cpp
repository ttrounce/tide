#define TIDE_CONTEXT_IMPL
#include "context.h"

#include "commands/keybinding.h"
#include "engine/engine.h"

#include <glfw/glfw3.h>

Context::Context()
{
    this->activityHolder = std::make_unique<ActivityHolder>();
    this->commandRegistry = std::make_unique<CommandRegistry>();
    this->commandPalette = std::make_unique<CommandPalette>(FontParams{ "hack", 10 });
    this->baseContainer = std::make_unique<PanelAncestor>(Rect{ 0,0, engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight() }, FontParams{ "hack", 2 });

    baseContainer->SetActive(true);
    baseContainer->SetFocusedNode(baseContainer->startNode.get());

    activityHolder->SetActiveComponent(baseContainer.get());
}

// Non member context functions below

namespace context
{

void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1);

    if(ctx->activityHolder->GetActiveComponent() == ctx->commandPalette.get())
    {
        ctx->commandPalette->Batch();
    }
    else if(ctx->activityHolder->GetActiveComponent() == ctx->baseContainer.get())
    {
        ctx->baseContainer->Batch();
    }
    
    std::string fpsText = fmt::format("{:.1f}ms {}fps", engine->window->frameTime, engine->window->fps);
    fontRenderer->BatchText("hack-10", fpsText, engine->GetWindowWidth() - fontRenderer->TextWidth("hack-10", fpsText) - 3, 5, 5.0, Color{ 0xFFFFFF });

    fontRenderer->Render();
}

void Update(double t, double dt)
{

}

void OnCharInput(unsigned int codepoint)
{
    if(ctx->activityHolder->GetActiveComponent() == ctx->baseContainer.get())
    {
        ctx->baseContainer->TakeInput(textpanel::INPUT_WRITE, codepoint);
    }
}

void OnKeyInput(int key, int action, int scancode)
{
    if (action == GLFW_PRESS)
    {
        if(ctx->activityHolder->GetActiveComponent() == ctx->commandPalette.get())
        {
            // this must go first incase selection actions cause a state change (such as keybing changing)
            ctx->commandPalette->TakeInput(palette::INPUT_KEY_CAPTURE, key);
        }
    }
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        // Standard keybindings (for text, and manipulating each component's UI easier)

        if(ctx->activityHolder->GetActiveComponent() == ctx->baseContainer.get())
        {
            if (key == GLFW_KEY_BACKSPACE)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_REMOVE_BACK, 0);
            }
            if (key == GLFW_KEY_DELETE)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_REMOVE_FWRD, 0);
            }
            if (key == GLFW_KEY_TAB)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_WRITE, '\t');
            }
            if (key == GLFW_KEY_ENTER)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_NEW_LINE, 0);
            }
            if (key == GLFW_KEY_UP)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_MOVE_UP, 0);
            }
            if (key == GLFW_KEY_DOWN)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_MOVE_DOWN, 0);
            }
            if (key == GLFW_KEY_LEFT)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_MOVE_LEFT, 0);
            }
            if (key == GLFW_KEY_RIGHT)
            {
                ctx->baseContainer->TakeInput(textpanel::INPUT_MOVE_RIGHT, 0);
            }
        }
        else if(ctx->activityHolder->GetActiveComponent() == ctx->commandPalette.get())
        {

            if (key == GLFW_KEY_UP)
            {
                ctx->commandPalette->TakeInput(palette::INPUT_MOVE_UP, 0);
            }
            if (key == GLFW_KEY_DOWN)
            {
                ctx->commandPalette->TakeInput(palette::INPUT_MOVE_DOWN, 0);
            }
            if(key == GLFW_KEY_ENTER)
            {
                ctx->commandPalette->TakeInput(palette::INPUT_SELECT, 0);
            }
        }

        // Command keybindings...

        const auto& commands = ctx->commandRegistry->GetCommands();
        const auto& keys = engine->window->keyboard->keys;
        for(const auto& pair : commands)
        {
            const auto& command = pair.second;
            if(command.keybinding.key == key)
            {
                bool ctrl = keys[GLFW_KEY_LEFT_CONTROL] || keys[GLFW_KEY_RIGHT_CONTROL];
                bool shft = keys[GLFW_KEY_LEFT_SHIFT  ] || keys[GLFW_KEY_RIGHT_SHIFT  ];
                bool meta = keys[GLFW_KEY_LEFT_ALT    ] || keys[GLFW_KEY_RIGHT_ALT    ];

                if (ctrl == command.keybinding.ctrl &&
                    shft == command.keybinding.shft &&
                    meta == command.keybinding.meta)
                {
                    ctx->commandRegistry->ExecuteCommand(command);
                }
            }
        }
    }
}

};