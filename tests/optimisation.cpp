#include "timer.h"
#include "engine/gfx/font.h"

#define TIDE_ENGINE_IMPLEMENTATION
#include "engine/engine.h"

void Init()
{
    InitFreeType();
    fontRenderer->LoadFace("hack", "hack.ttf", 16);
}

void Update(double t, double dt)
{

}

void Draw()
{

}

int main()
{
    engine = std::make_unique<Engine>(800, 600, "Timing Tests");
    if (engine->GetStatus())
    {
        Init();
        engine->Start(Update, Draw);
    }
    FreeFreeType();
    return 0;
}