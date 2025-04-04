#include "Surface.h"

#include "Instance.h"
#include "Engine.h"
#include <GLFW/glfw3.h>

constexpr VkFormat priorities[] = {
    VK_FORMAT_R8G8B8A8_UNORM,
    VK_FORMAT_R8G8B8A8_SRGB,
};

Surface::Surface(Scope *scope) : scope(scope) {}

void Surface::Create()
{
    VkInstance instance = scope().getInstance().getHandle();
    GLFWwindow *window = scope().getWindow().getHandle();
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &handle);
    if (result != VK_SUCCESS)
        panic("Couldn't create surface");
}

std::vector<VkSurfaceFormatKHR> Surface::GetFormats()
{
    VkPhysicalDevice physicalDevice = scope().getPhyDevice().getHandle();
    uint32_t count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, handle, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, handle, &count, formats.data());
    return formats;
}

void Surface::Destroy()
{
    VkInstance instance = scope().getInstance().getHandle();
    vkDestroySurfaceKHR(instance, handle, nullptr);
}

VkSurfaceCapabilitiesKHR Surface::GetCaps()
{
    VkPhysicalDevice physicalDevice = scope().getPhyDevice().getHandle();
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, handle, &caps);
    return caps; 
}

VkSurfaceFormatKHR Surface::GetBestFormat()
{
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
