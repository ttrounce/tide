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
    Timer<std::chrono::microseconds> timer;

    for(int i = 0; i < 1000; i++)
    {
        fontRenderer->RenderText("hack", "test123", 0, 0, 0, Color(0xFF0000)); 
    }
}

int main()
{
    engine = std::make_unique<Engine>(800, 600, "Timing Tests");
    if(engine->GetStatus())
    {
        Init();
        engine->Start(Update, Draw);
    }
    FreeFreeType();
    
    return 0;
}