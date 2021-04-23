#include "window.h"
#include "engine.h"

#include <glm/ext/scalar_common.hpp>
#include <glad/glad.h>
#include <fmt/core.h>
#include <chrono>
#include <thread>

namespace tide
{

static void ErrorCallbackGLFW(int error, const char* description)
{
    fmt::print("[TIDE] {}\n", description);
}

ENGINE::ENGINE(int initialWidth, int initialHeight, std::string title)
{    
    int glfwError = glfwInit();

    glfwSetErrorCallback(ErrorCallbackGLFW);

    if(glfwError == GLFW_FALSE)
    {
        return;
    }

    this->window = std::make_unique<WINDOW>();
    this->window->handle = glfwCreateWindow(initialWidth, initialHeight, title.c_str(), 0, 0);
    if(this->window == NULL)
    {
        return;
    }
    glfwMakeContextCurrent(window->handle);

    int gladError = gladLoadGL();
    if(gladError == GLFW_FALSE)
    {
        return;
    }
    window->keyboard = std::make_unique<KEYBOARD>();
    window->mouse = std::make_unique<MOUSE>();

    for(int i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++)
    {
        window->mouse->buttons[i] = false;
    }
    for(int i = 0; i < GLFW_KEY_LAST; i++)
    {
        window->keyboard->keys[i] = false;
    }
    glfwSetCursorPosCallback(window->handle, [](GLFWwindow* handle, double x, double y){
        engine->window->mouse->pos = {x, y};
    });
    glfwSetWindowSizeCallback(window->handle, [](GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
        for(auto it = engine->resizeListeners.begin(); it != engine->resizeListeners.end(); it++)
        {
            resize_listener rl = *it;
            rl(width, height);
        }
    });
    glfwSetFramebufferSizeCallback(window->handle, [](GLFWwindow* window, int width, int height){
        for(auto it = engine->resizeFrameBufferListeners.begin(); it != engine->resizeFrameBufferListeners.end(); it++)
        {
            resize_listener rl = *it;
            rl(width, height);
        }
    });

    glfwSetKeyCallback(window->handle, [](GLFWwindow* window, int k, int s, int a, int m){   
        for(auto it = engine->keyListeners.begin(); it != engine->keyListeners.end(); it++)
        {
            key_listener kl = *it;
            kl(k, a, s);
        }
    });

    glfwSetCharCallback(window->handle, [](GLFWwindow* window, uint codepoint){   
        for(auto it = engine->charListeners.begin(); it != engine->charListeners.end(); it++)
        {
            char_listener cl = *it;
            cl(codepoint);
        }
    });

    this->success = true;
}

using namespace std::chrono_literals;

void ENGINE::Start(void (*update)(double, double), void (*draw)())
{
    const double dt = 1.0/60.0;
    double updateAccumulator = 0.0;
    double frameCountAccumulator = 0.0;
    
    double time = 0.0;
    
    auto startTime = std::chrono::high_resolution_clock::now();

    double frameTime = 0.0;
    int frameCount = 0;        

    glfwSwapInterval(1);

    while(!glfwWindowShouldClose(window->handle))
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        frameTime = std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime)).count();
        startTime = endTime;
        
        glfwPollEvents();

        updateAccumulator += frameTime;
        frameCountAccumulator += frameTime;
        
        if(updateAccumulator >= dt)
        {
            updateAccumulator -= dt;
            time += dt;

            // mouse delta calculations
            window->mouse->delta = glm::clamp(window->mouse->pos - window->mouse->lastPos, glm::vec2(-128.0f), glm::vec2(128.0f));
            window->mouse->lastPos = window->mouse->pos;

            update(time, dt);
        }
        
        if(frameCountAccumulator >= 1.0)
        {
            window->fps = glm::max(frameCount, 0);
            frameCount = 0;
            frameCountAccumulator -= 1;
        }

        
        frameCount++;
        draw();
        glfwSwapBuffers(window->handle);
    } 
}

ENGINE::~ENGINE()
{
    glfwTerminate();
}

}