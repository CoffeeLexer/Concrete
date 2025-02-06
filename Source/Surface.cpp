#include "Surface.h"

#include "Instance.h"
#include "Engine.h"

namespace
{
    const VkFormat priorities[] = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SRGB,
    };
}

Surface::Surface(Engine &engine)
    : engine(engine)
{
    Window &window = engine;
    surface = window.CreateSurface();
}

Surface::~Surface()
{
    VkInstance &instance = engine;
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

Surface::operator VkSurfaceKHR&()
{
    return surface;
}

VkSurfaceCapabilitiesKHR Surface::GetCaps()
{
    VkPhysicalDevice physicalDevice = engine;
    VkSurfaceKHR surface = engine;
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
    return caps; 
}

VkSurfaceFormatKHR Surface::GetBestFormat()
{
    VkPhysicalDevice physicalDevice = engine;
    VkSurfaceKHR surface = engine;
    uint32_t count;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());

    for (const auto& p : priorities)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (p == formats.at(i).format)
                return formats.at(i);
        }
    }
    return formats.at(0);
}
