#include "command.h"

#include "../engine/engine.h"
#include "../context.h"

#include <glfw/glfw3.h>
#include <fmt/core.h>

void CommandRegistry::AddCommand(const Command& command)
{
    commands.insert(std::pair<std::string, Command>{command.commandName, command});
}

void CommandRegistry::ExecuteCommand(const std::string& key)
{
    if(commands.count(key) != 0)
    {
        auto& command = commands.at(key);
        fmt::print("Executing command: {}\n", command.commandName);
        if(command.shouldClosePalette)
        {
            commands.at("close_palette").func();
        }
        command.func();
    }
    // TODO: think about fail case
}

void CommandRegistry::ExecuteCommand(const Command& command)
{
    fmt::print("Executing command: {}\n", command.commandName);
    if(command.shouldClosePalette)
    {
        commands.at("close_palette").func();
    }
    command.func();
}

void CommandRegistry::SetCommandKeybind(const std::string& key, const Keybinding& keybinding)
{
    if(commands.count(key) != 0)
    {
        commands.at(key).keybinding = keybinding;
    }
}

int CommandRegistry::GetCommandsCount()
{
    return commands.size();
}

Keybinding CommandRegistry::GetCommandKey(std::string key)
{
    if(commands.count(key))
    {
        return commands.at(key).keybinding;
    }
    return {};
}

std::map<std::string, Command> CommandRegistry::GetCommands()
{
    return commands;
}

CommandRegistry::CommandRegistry()
{
    AddCommands();
}

#define CMD_CLOSE_FCS_PANEL "close_focused_panel"
#define CMD_CLOSE_PALETTE "close_palette"
#define CMD_OPEN_PALETTE "open_palette"
#define CMD_SPLIT_FCS_VERT "split_focused_vert"
#define CMD_SPLIT_FCS_HORZ "split_focused_horz"
#define CMD_PRINT_PANEL_TREE "print_panel_tree"
#define CMD_CYCLE_FCS_PANEL "cycle_focused_panel"
#define CMD_MODIFY_KEYBINDS "modify_keybinds"

void CommandRegistry::AddCommands()
{
    this->AddCommand({
        CMD_CLOSE_FCS_PANEL,
        "Close Focused Panel",
        []() {
            ctx->baseContainer->CloseFocusedNode();
            ctx->baseContainer->UpdateFrameSize(engine->GetFrameBufferWidth(), engine->GetFrameBufferHeight());
        },
        Keybinding { true, false, false, GLFW_KEY_W },
        true
    });

    this->AddCommand({
        CMD_CLOSE_PALETTE,
        "Close Palette",
        []() {
            ctx->activityHolder->SetActiveComponent(ctx->baseContainer.get());
        },
        Keybinding { false, false, false, GLFW_KEY_ESCAPE },
        false
    });

    this->AddCommand({
        CMD_OPEN_PALETTE,
        "Open Palette", 
        []() {
            ctx->activityHolder->SetActiveComponent(ctx->commandPalette.get());
        },
        Keybinding { true, true, false, GLFW_KEY_P },
        false
    });

    this->AddCommand({
        CMD_SPLIT_FCS_VERT,
        "Split Focused Panel (Vertical)",
        []() {
            ctx->baseContainer->SplitFocusedVert();
        },
        Keybinding { true, false, false, GLFW_KEY_RIGHT_BRACKET },
        true
    });

    this->AddCommand({
        CMD_SPLIT_FCS_HORZ,
        "Split Focused Panel (Horizontal)",
        []() {
            ctx->baseContainer->SplitFocusedHorz();
        },
        Keybinding { true, false, false, GLFW_KEY_LEFT_BRACKET },
        true
    });

    this->AddCommand({
        CMD_PRINT_PANEL_TREE,
        "Print Panel Tree (Debug)",
        []() {
            container::PrintPanelTree(ctx->baseContainer.get());
        },
        Keybinding { false, false, false, GLFW_KEY_F5 },
    });

    this->AddCommand({
        "close_tide",
        "Exit",
        []() {
            glfwSetWindowShouldClose(engine->window->handle, GLFW_TRUE);
        },
        false
    });

    this->AddCommand({
        CMD_CYCLE_FCS_PANEL,
        "Cycle Focused Panel",
        []() {
            ctx->baseContainer->CycleFocus();
        },
        Keybinding { true, false, false, GLFW_KEY_I },
        true
    });

    this->AddCommand({
        CMD_MODIFY_KEYBINDS,
        "Modify Keybinds",
        []() {

        },
        false
    });
}