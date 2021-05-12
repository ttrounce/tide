#include "commandpalette.h"

#include "../../engine/engine.h"
#include "../../engine/gfx/font.h"
#include "../../context.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

CommandPalette::CommandPalette(const FontParams& fontParams) : fontParams(fontParams)
{
    this->selector = 0;
}

void CommandPalette::TakeInput(int inputType, unsigned int codepoint)
{
    switch(inputType)
    {
        case palette::INPUT_MOVE_DOWN:
        {
            this->selector += 1;
            if(selector >= static_cast<int>(ctx->commandRegistry->GetCommandsCount())) selector = 0;
            break;
        }
        case palette::INPUT_MOVE_UP:
        {
            this->selector = selector - 1;
            if(selector < 0) selector = ctx->commandRegistry->GetCommandsCount() - 1;
            break;
        }
        case palette::INPUT_SELECT:
        {
            auto& command = std::next(ctx->commandRegistry->GetCommands().begin(), selector)->second;
            ctx->commandRegistry->ExecuteCommand(command);
            break;
        }
        case palette::INPUT_KEY_CAPTURE:
        {
            break;
        }
        default:
        {
            break;
        }
    }
}

std::string GetKeyName(int key)
{
    const char* name = glfwGetKeyName(key, 0);
    if(name)
    {
        std::string str = std::string(name);
        transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }
    switch(key)
    {   
        // Function Keys
        case GLFW_KEY_F1: return "F1";
        case GLFW_KEY_F2: return "F2";
        case GLFW_KEY_F3: return "F3";
        case GLFW_KEY_F4: return "F4";
        case GLFW_KEY_F5: return "F5";
        case GLFW_KEY_F6: return "F6";
        case GLFW_KEY_F7: return "F7";
        case GLFW_KEY_F8: return "F8";
        case GLFW_KEY_F9: return "F9";
        case GLFW_KEY_F10: return "F10";
        case GLFW_KEY_F11: return "F11";
        case GLFW_KEY_F12: return "F12";
        // Other, Non-Character Keys
        case GLFW_KEY_SPACE: return "Space";
        case GLFW_KEY_ESCAPE: return "Escape";
        case GLFW_KEY_ENTER: return "Enter";
        case GLFW_KEY_RIGHT_SHIFT:
        case GLFW_KEY_LEFT_SHIFT: return "Shift";
        case GLFW_KEY_RIGHT_CONTROL:
        case GLFW_KEY_LEFT_CONTROL: return "Ctrl";
        case GLFW_KEY_RIGHT_ALT:
        case GLFW_KEY_LEFT_ALT: return "Alt";
        default: return "N/A";
    }
}

void CommandPalette::Batch()
{
    const auto frameWidth = engine->GetFrameBufferWidth();
    const auto frameHeight = engine->GetFrameBufferHeight();
    const auto lineHeight = fontParams.GetLineHeight();

    fontRenderer->BatchRect(0, 0, frameWidth, frameHeight, 0.5, Color(0x000000));
    fontRenderer->BatchText("hack-10", "[Up,Down] to cycle, [Enter] to select", 5, 15, 0.49, Color(0xFFFFFF));
    int commandStartY = lineHeight;

    int i = 0;
    for(const auto& pair : ctx->commandRegistry->GetCommands())
    {
        const Command& command = pair.second;
        
        const auto& keybind = command.keybinding;
        std::string keyName = "";
        if(keybind.ctrl) keyName += "Ctrl+";
        if(keybind.shft) keyName += "Shift+";
        if(keybind.meta) keyName += "Alt+";
        if(keybind.key != -1) keyName += GetKeyName(keybind.key); 

        auto keybindText = keyName;
        auto backgroundColor = Color(0x0A0A0A);
        auto textColor = Color(0x777777);
        if(selector == i)
        {
            backgroundColor = Color(0x161616);
            textColor = Color(0xFFFFFF);
        }

        fontRenderer->BatchRect(0, i * lineHeight + commandStartY, frameWidth, lineHeight, 0.5, backgroundColor);
        fontRenderer->BatchText(fontParams.fontName, keybindText, frameWidth - fontRenderer->TextWidth(fontParams.fontName, keybindText) - fontParams.linePadding, i * lineHeight + commandStartY + fontParams.linePadding, 0.49, textColor);
        fontRenderer->BatchText(fontParams.fontName, command.commandLabel, fontParams.linePadding, i * lineHeight + commandStartY + fontParams.linePadding, 0.49, textColor);
        i++;
    }
}