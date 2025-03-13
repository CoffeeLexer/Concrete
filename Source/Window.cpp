#include "Window.h"

#include <cstdio>
#include <cstdlib>

#include "GLFW/glfw3.h"
#include "Instance.h"
#include "Engine.h"
#include "Panic.h"

uint32_t globalInstanceCount = 0;

void CallbackError(int err, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void CallbackFramebufferSize(GLFWwindow *window, int width, int height)
{
    auto &userData = GetUserDataRef(window);
    userData.SetFramebufferSize(width, height);
}

void GlobalInit()
{
    globalInstanceCount++;
    if (globalInstanceCount > 1) return;

    if (!glfwInit())
    {
        printf("Failed GLFW INIT");
        exit(1);
    }

    glfwSetErrorCallback(CallbackError);
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
    {
        fprintf(stderr, "Failed GLFWwindow creation\n");
        exit(1);
    }

    // glfwSetWindowUserPointer(handle, &userData);
    glfwSetKeyCallback(handle, CallbackKey);
    glfwSetFramebufferSizeCallback(handle, CallbackFramebufferSize);
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
