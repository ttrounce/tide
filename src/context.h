#ifndef TIDE_CONTEXT_H
#define TIDE_CONTEXT_H

#include "components/textpanel/container.h"
#include "components/palette/commandpalette.h"
#include "components/activityholder.h"
#include "commands/command.h"

struct Context
{
    Unique<ActivityHolder> activityHolder;
    Unique<CommandRegistry> commandRegistry;
    Unique<CommandPalette> commandPalette;
    Unique<PanelAncestor> baseContainer;

    Context();
};

extern Unique<Context> ctx;

#ifdef TIDE_CONTEXT_IMPL
Unique<Context> ctx;
#endif

namespace context
{
    void OnCharInput(unsigned int codepoint);
    void OnKeyInput(int key, int action, int scancode);
    void Draw();
    void Update(double t, double dt);
};

#endif // TIDE_CONTEXT_H