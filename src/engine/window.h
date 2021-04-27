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

    Window()
    {
        this->fps = 0;
    }
};

typedef void (*key_listener)(int, int, int);
typedef void (*char_listener)(uint);
typedef void (*resize_listener)(int, int);

class Engine
{
    bool success;
public:
    Unique<Window> window;
    std::vector<key_listener> keyListeners;
    std::vector<char_listener> charListeners;
    std::vector<resize_listener> resizeListeners;
    std::vector<resize_listener> resizeFrameBufferListeners;

    Engine(int initialWidth, int initialHeight, const std::string& title);
    ~Engine();

    void Start(void (*update)(double, double), void (*draw)());
    bool GetStatus()
    {
        return success;
    }
    int GetWindowWidth()
    {
        int width;
        glfwGetWindowSize(window->handle, &width, NULL);
        return width;
    }
    int GetWindowHeight()
    {
        int height;
        glfwGetWindowSize(window->handle, NULL, &height);
        return height;
    }
    int GetFrameBufferWidth()
    {
        int width;
        glfwGetFramebufferSize(window->handle, &width, NULL);
        return width;
    }
    int GetFrameBufferHeight()
    {
        int height;
        glfwGetFramebufferSize(window->handle, NULL, &height);
        return height;
    }
};



#endif // TIDE_WINDOW_H