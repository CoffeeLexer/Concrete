#pragma once

#include "Handle.h"
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
    Info userData = {};

    void createWindow();
public:
    explicit Window(Scope &scope);
    ~Window();

    Handle<GLFWwindow*> getWindow{window};
    Handle<Info> getInfo{userData};

    static void PollEvents();
    [[nodiscard]] bool IsValid() const;
};

namespace Callback
{
    void Error(int err, const char* description);
    void Key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void FramebufferSize(GLFWwindow *window, int width, int height);
}
