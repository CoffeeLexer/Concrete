#pragma once

#include <vulkan/vulkan.h>

class GLFWwindow;
class Scope;
struct Info
{
    int width = 0;
    int height = 0;
};

class Window
{
    Scope &scope;
    GLFWwindow *window;
    VkSurfaceKHR surface;
    Info userData = {};

    void createWindow();
    void createSurface();

public:
    explicit Window(Scope &scope);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    const GLFWwindow* getWindow() { return window; }
    const VkSurfaceKHR& getVkSurface() { return surface; }
    const Info& getInfo() { return userData; }

    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};

namespace Callback
{
    void Error(int err, const char* description);
    void Key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void FramebufferSize(GLFWwindow *window, int width, int height);
}
