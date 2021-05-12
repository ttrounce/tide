#ifndef TIDE_COMMAND_H
#define TIDE_COMMAND_H

#include "keybinding.h"

#include <map>
#include <string>
#include <functional>

struct Command
{
    std::string commandName;
    std::string commandLabel;
    std::function<void()> func;
    Keybinding keybinding;
    bool shouldClosePalette;
};

// Forward decl to reduce circular include
struct Context;

class CommandRegistry
{
public:
    void AddCommand(const Command& command);
    void ExecuteCommand(const std::string& key);
    void ExecuteCommand(const Command& command);
    void SetCommandKeybind(const std::string& key, const Keybinding& keybinding);

    int GetCommandsCount();

    std::map<std::string, Command> GetCommands();
    Keybinding GetCommandKey(std::string key);

    CommandRegistry();
private:
    std::map<std::string, Command> commands;
    void AddCommands();
};



#endif // TIDE_COMMAND_H