#ifndef TIDE_PALETTE_H
#define TIDE_PALETTE_H

#include "../../commands/keybinding.h"
#include "../../commands/command.h"
#include "../../engine/types.h"
#include "../fontparams.h"
#include "../component.h"

namespace palette
{

enum InputType
{
    INPUT_SELECT,
    INPUT_MOVE_UP,
    INPUT_MOVE_DOWN,
    INPUT_KEY_CAPTURE
};

}

class CommandPalette : public ActableComponent
{
private:
    const FontParams fontParams;
    int selector;
public:
    CommandPalette(const FontParams& fontParams);

    virtual void TakeInput(int inputType, unsigned int codepoint);
    virtual void Batch();
};


#endif // TIDE_PALETTE_H