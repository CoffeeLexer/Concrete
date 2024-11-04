#include "Window.h"

#include "GLFW/glfw3.h"

namespace {
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

Window::Window()
{
    Window::GlobalInit();

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

    glfwSetKeyCallback(window, CallbackKey);
}

Window::~Window()
{
    glfwDestroyWindow(window);
    Window::GlobalTerminate();
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
    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Couldn't create surface");
    }
    return surface;
}