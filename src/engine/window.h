#ifndef TIDE_WINDOW_H
#define TIDE_WINDOW_H

#include "types.h"

#include <glm/vec2.hpp>

#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>
#include <vector>
#include <memory>

struct Keyboard
{
    bool keys[GLFW_KEY_LAST];
};

struct Mouse
{
    bool buttons[GLFW_MOUSE_BUTTON_LAST];
    glm::vec2 delta;
    glm::vec2 pos;
    glm::vec2 lastPos;
};

struct Window
{
    GLFWwindow* handle;

    Unique<Keyboard> keyboard;
    Unique<Mouse>    mouse;

    bool iconified;
    uint fps;
    double frameTime;

    int frameRateTarget;

    Window();
};

using key_listener = void (*)(int, int, int);
using char_listener = void (*)(uint);
using resize_listener = void (*)(int, int);

using update_func = void (*)(double, double);
using draw_func = void (*)();

class Engine
{
public:
    Unique<Window> window;
    std::vector<key_listener> keyListeners;
    std::vector<char_listener> charListeners;
    std::vector<resize_listener> resizeListeners;
    std::vector<resize_listener> resizeFrameBufferListeners;

    Engine(int initialWidth, int initialHeight, const std::string& title);
    ~Engine();

    void Start(update_func update, draw_func draw);
    bool GetStatus();
    int GetWindowWidth();
    int GetWindowHeight();
    int GetFrameBufferWidth();
    int GetFrameBufferHeight();
private:
    bool success;
};



#endif // TIDE_WINDOW_H