#pragma once

#include "vulkan/vulkan.h"

class Engine;
class GLFWwindow;

class Window
{
    Engine &engine;
    GLFWwindow *window;

public:

    Window(Engine &engine);
    ~Window();

    void SwapBuffers();
    void PollEvents();
    bool IsValid();

    VkSurfaceKHR CreateSurface();
};