#include <stdexcept>
#include "Window.h"

#include "GLFW/glfw3.h"
#include "Scope.h"
#include "Panic.h"

static uint32_t globalInstanceCount = 0;

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

Window::Window(Scope &scope) : scope(scope)
{
    createWindow();
    createSurface();
}

void Window::createWindow()
{
    GlobalInit();
    window = glfwCreateWindow(480, 480, "Concrete Window", nullptr, nullptr);
    userData.height = 480;
    userData.width = 480;

    if (!window)
        panic("Failed GLFWwindow creation");

    glfwSetWindowUserPointer(window, &userData);
    glfwSetKeyCallback(window, Callback::Key);
    glfwSetFramebufferSizeCallback(window, Callback::FramebufferSize);
}

Window::~Window()
{
    const auto instance = scope.getInstance().getVkInstance();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    glfwDestroyWindow(window);
    GlobalTerminate();
}

void Window::PollEvents()
{
    glfwPollEvents();
}

bool Window::IsValid() const
{
    return !glfwWindowShouldClose(window);
}


void Window::createSurface()
{
    const auto instance = scope.getInstance().getVkInstance();
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS)
        throw std::runtime_error("VkSurface::Create: Failed");
}
