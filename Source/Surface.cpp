#include "Surface.h"

#include "Instance.h"
#include "Engine.h"
#include <GLFW/glfw3.h>

constexpr VkFormat priorities[] = {
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SRGB,
};

Surface::Surface(Engine *engine)
    : Link(engine)
{
    VkInstance instance = Owner().instance;
    GLFWwindow *window = Owner().window;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &handle);
    if (result != VK_SUCCESS)
        panic("Couldn't create surface");
}


std::vector<VkSurfaceFormatKHR> Surface::GetFormats()
{
    VkPhysicalDevice physicalDevice = Owner().physicalDevice;
    uint32_t count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, handle, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, handle, &count, formats.data());
    return formats;
}

Surface::~Surface()
{
    VkInstance instance = Owner().instance;
    vkDestroySurfaceKHR(instance, handle, nullptr);
}

VkSurfaceCapabilitiesKHR Surface::GetCaps()
{
    VkPhysicalDevice physicalDevice = Owner().physicalDevice;
    VkSurfaceKHR surface = Owner().surface;
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
    return caps; 
}

VkSurfaceFormatKHR Surface::GetBestFormat()
{
    VkPhysicalDevice physicalDevice = Owner().physicalDevice;
    auto formats = GetFormats();

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
