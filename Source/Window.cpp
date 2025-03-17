#include "Window.h"

#include <cstdio>
#include <cstdlib>

#include "GLFW/glfw3.h"
#include "Instance.h"
#include "Engine.h"
#include "Panic.h"

uint32_t globalInstanceCount = 0;

class UserData
{
    VkExtent2D frameBuffer;
public:
    void SetFrameBufferSize(int width, int height)
    {
        frameBuffer = {
            .width = static_cast<uint32_t>(width),
            .height = static_cast<uint32_t>(height),
        };
    }
};

namespace Callback
{
    void Error(int err, const char* description);
    void Key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void FramebufferSize(GLFWwindow *window, int width, int height);
}

UserData& GetUserData(GLFWwindow *window)
{
    auto *ptr = glfwGetWindowUserPointer(window);
    return *static_cast<UserData*>(ptr);
}

void Callback::Error(int err, const char* description)
{
    fprintf(stderr, "GLFW Error: %s\n", description);
}

void Callback::Key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Callback::FramebufferSize(GLFWwindow *window, int width, int height)
{
    auto &userData = GetUserData(window);
    userData.SetFrameBufferSize(width, height);
}

void GlobalInit()
{
    globalInstanceCount++;
    if (globalInstanceCount > 1) return;

    if (!glfwInit())
        panic("Failed GLFW INIT");

    glfwSetErrorCallback(Callback::Error);
}

void GlobalTerminate()
{
    globalInstanceCount--;

    if (globalInstanceCount > 0) return;

    glfwTerminate();
}

Window::Window(Engine *engine)
    : Link(engine)
{
    GlobalInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    handle = glfwCreateWindow(480, 480, "Concrete Window", nullptr, nullptr);

    if (!handle)
        panic("Failed GLFWwindow creation");

    userData = new UserData;
    glfwSetWindowUserPointer(handle, userData);
    glfwSetKeyCallback(handle, Callback::Key);
    glfwSetFramebufferSizeCallback(handle, Callback::FramebufferSize);
}

Window::~Window()
{
    glfwDestroyWindow(handle);
    GlobalTerminate();
}

void Window::PollEvents()
{
    glfwPollEvents();
}

bool Window::IsValid() const
{
    return !glfwWindowShouldClose(handle);
}
