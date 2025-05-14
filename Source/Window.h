#pragma once

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"
#include <vulkan/vulkan.h>

class Engine;
class GLFWwindow;

struct UserData
{
    int width = 0;
    int height = 0;
};

class Window
{
    Scope &scope;
    GLFWwindow *window;
    VkSurfaceKHR surface;

    UserData userData = {};

    void createWindow();
    void createSurface();
public:
    explicit Window(Scope &scope);
    ~Window();

    Handle<GLFWwindow*> getWindow{window};
    Handle<VkSurfaceKHR> getSurface{surface};

    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};

namespace Callback
{
    void Error(int err, const char* description);
    void Key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void FramebufferSize(GLFWwindow *window, int width, int height);
}
