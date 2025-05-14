#include "Window.h"

#include "GLFW/glfw3.h"
#include "Instance.h"
#include "Engine.h"
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
}

void Window::createWindow()
{
    GlobalInit();
    window = glfwCreateWindow(480, 480, "Concrete Window", nullptr, nullptr);

    if (!window)
        panic("Failed GLFWwindow creation");

    glfwSetWindowUserPointer(window, &userData);
    glfwSetKeyCallback(window, Callback::Key);
    glfwSetFramebufferSizeCallback(window, Callback::FramebufferSize);
}

Window::~Window()
{
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

constexpr VkFormat priorities[] = {
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SRGB,
};
struct SurfaceFormats : std::vector<VkSurfaceFormatKHR> {
    SurfaceFormats(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
        uint32_t count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
        this->resize(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, this->data());
    }
};

struct SurfaceCaps : VkSurfaceCapabilitiesKHR {
    SurfaceCaps(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, this);
    }
};

VkSurfaceFormatKHR Window::selectBestFormat()
{
    auto formats = SurfaceFormats{scope.getDevice().getVkPhysicalDevice(), surface};
    for (const auto& p : priorities)
    {
        for (const auto& format : formats)
        {
            if (p == format.format)
                return format;
        }
    }
    return formats.at(0);
}
