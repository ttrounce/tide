#define TIDE_ENGINE_IMPLEMENTATION
#include "engine/engine.h"

#include <fmt/core.h>
#include <fmt/printf.h>

#include <pthread.h>

#include "components/textpanel/textpanel.h"
#include "context.h"

#include "engine/gfx/font.h"
#include "engine/gfx/gfx.h"

#include "prefab/model.h"

#include "engine/types.h"


void Draw()
{

}

int main()
{
    engine = std::make_unique<Engine>(800, 600, "T.IDE");
    if (engine->GetStatus())
    {
        fmt::print("GL Version: {}\n", glGetString(GL_VERSION));

        ctx = std::make_unique<Context>();

        engine->window->frameRateTarget = 60;
        engine->charListeners.push_back(context::OnCharInput);
        engine->keyListeners.push_back(context::OnKeyInput);
        engine->resizeFrameBufferListeners.push_back([](int width, int height) {
            fontRenderer->UpdateOrthographic(width, height);
            ctx->baseContainer->UpdateFrameSize(width, height);
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

        engine->Start(context::Update, context::Draw);
    }
    else
    {
        fmt::print("[TIDE] Engine failed to build\n");
    }
    FreeFreeType();
}