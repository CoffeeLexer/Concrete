#pragma once

#include "vulkan/vulkan.h"

class Engine;
class GLFWwindow;

struct WindowUserData
{
    VkExtent2D extent;
    WindowUserData();
    void SetFramebufferSize(int width, int height);
};

class Window
{
    Engine &engine;
    GLFWwindow *window;
    WindowUserData userData;
public:

    Window(Engine &engine);
    ~Window();

    void SwapBuffers();
    void PollEvents();
    bool IsValid();

    VkSurfaceKHR CreateSurface();

    operator GLFWwindow*();
};
