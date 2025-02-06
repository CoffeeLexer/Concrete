#include "Window.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include "GLFW/glfw3.h"

#include "Instance.h"
#include "Engine.h"

WindowUserData::WindowUserData()
{
    extent = {0, 0};
}

void WindowUserData::SetFramebufferSize(int width, int height)
{
    extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
    };
}

namespace {
    uint32_t globalInstanceCount = 0;

    WindowUserData& GetUserDataRef(GLFWwindow *window)
    {
        auto ptr = glfwGetWindowUserPointer(window);
        return *reinterpret_cast<WindowUserData*>(ptr);
    }

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
}

Window::Window(Engine &engine)
    : engine(engine)
{
    GlobalInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(480, 480, "Concrete Window", nullptr, nullptr);

    if (!window)
    {
        fprintf(stderr, "Failed GLFWwindow creation\n");
        exit(1);
    }

    glfwSetWindowUserPointer(window, &userData);
    glfwSetKeyCallback(window, CallbackKey);
    glfwSetFramebufferSizeCallback(window, CallbackFramebufferSize);
}

Window::~Window()
{
    glfwDestroyWindow(window);
    GlobalTerminate();
}

Watcher<VkExtent2D> Window::BindExtent()
{
    return {userData.extent};
}

void Window::SwapBuffers()
{
    // OpenGL implementation only
    // glfwSwapBuffers(window);
}

void Window::PollEvents()
{
    glfwPollEvents();
}

bool Window::IsValid()
{
    return !glfwWindowShouldClose(window);
}

VkSurfaceKHR Window::CreateSurface()
{
    VkInstance &instance = engine;
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't create surface");
    }
    return surface;
}