#include "Swapchain.h"

#include <stdexcept>

#include "Engine.h"

VkPresentModeKHR Swapchain::GetBestPresentMode()
{
    uint32_t count;
    const VkSurfaceKHR &surface = engine;
    const VkPhysicalDevice &physicalDevice = engine;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    std::vector<VkPresentModeKHR> modes = {};
    modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());

    VkPresentModeKHR priorities[] = {
        VK_PRESENT_MODE_FIFO_KHR,
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
    };

    for (const auto& p : priorities)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (p == modes.at(i))
                return p;
        }
    }
    return modes.at(0);
}

VkSurfaceFormatKHR Swapchain::GetBestSurfaceFormat()
{
    VkPhysicalDevice physicalDevice = engine;
    VkSurfaceKHR surface = engine;
    uint32_t count;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());

    VkFormat priorities[] = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SRGB,
    };

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


VkSurfaceCapabilitiesKHR Swapchain::GetSurfaceCaps()
{
    VkPhysicalDevice physicalDevice = engine;
    VkSurfaceKHR surface = engine;
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
    return caps; 
}

Swapchain::Swapchain(Engine &engine)
    : engine(engine)
{
    VkPresentModeKHR presentMode = GetBestPresentMode();
    const auto caps = GetSurfaceCaps();
    VkSurfaceFormatKHR format = GetBestSurfaceFormat();
    VkSurfaceKHR surface = engine;
    VkSwapchainCreateInfoKHR ci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = caps.minImageCount,
        .imageFormat = format.format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = caps.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = caps.currentTransform,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = nullptr,
    };

    ci.compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(
        caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
    
    Device &deviceWrapper = engine;
    uint32_t graphicsIndex = deviceWrapper.GetGraphicsIndex();
    uint32_t presentIndex = deviceWrapper.GetPresentIndex();
    uint32_t queueIndices[] = {graphicsIndex, presentIndex};

    if (graphicsIndex == presentIndex)
    {
        ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        ci.queueFamilyIndexCount = 0;
        ci.pQueueFamilyIndices = nullptr;
    }
    else
    {
        ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        ci.queueFamilyIndexCount = 2;
        ci.pQueueFamilyIndices = queueIndices;
    }

    VkDevice device = deviceWrapper;
    VkResult result = vkCreateSwapchainKHR(device, &ci, nullptr, &swapchain);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed creating swapchain");
    }
}

Swapchain::~Swapchain()
{
    VkDevice device = engine;
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}
