#include "Window.h"

#include <cstdio>
#include <cstdlib>

#include "GLFW/glfw3.h"
#include "Instance.h"
#include "Engine.h"
#include "Panic.h"

uint32_t globalInstanceCount = 0;

namespace Callback
{
    void Error(int err, const char* description);
    void Key(GLFWwindow* window, int key, int scancode, int action, int mods);
    void FramebufferSize(GLFWwindow *window, int width, int height);
}

struct UserData
{
    int width = 0;
    int height = 0;
};

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

void Callback::FramebufferSize(GLFWwindow *window, const int width, const int height)
{
    auto &userData = GetUserData(window);
    userData.width = width;
    userData.height = height;
}

void GlobalInit()
{
    globalInstanceCount++;
    if (globalInstanceCount > 1) return;

    if (!glfwInit())
        panic("Failed GLFW INIT");

    glfwSetErrorCallback(Callback::Error);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

void GlobalTerminate()
{
    globalInstanceCount--;

    if (globalInstanceCount > 0) return;

    glfwTerminate();
}

void Window::Create()
{
    GlobalInit();

    handle = glfwCreateWindow(480, 480, "Concrete Window", nullptr, nullptr);

    if (!handle)
        panic("Failed GLFWwindow creation");

    userData = new UserData;
    glfwSetWindowUserPointer(handle, userData);
    glfwSetKeyCallback(handle, Callback::Key);
    glfwSetFramebufferSizeCallback(handle, Callback::FramebufferSize);
}

void Window::Destroy()
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
