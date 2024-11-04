#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "Engine.h"

// class CommandPool
// {
//     friend CommandBuffer;

//     VkCommandPool pool;
//     const Device &device;
// public:
//     CommandPool(const Device &device)
//         : device(device)
//     {
//         VkCommandPoolCreateInfo ci = {
//             .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
//             .pNext = nullptr,
//             .flags = 0,
//             .queueFamilyIndex = device.graphicsIndex,
//         };

//         VkResult result = vkCreateCommandPool(device, &ci, nullptr, &pool);
//         if (result != VK_SUCCESS)
//         {
//             throw std::runtime_error("Command Pool init failed");
//         }
//     }

//     ~CommandPool()
//     {
//         vkDestroyCommandPool(device, pool, nullptr);
//     }
// };

// class CommandBuffer
// {
//     const CommandPool &pool;
//     VkCommandBuffer buffer;
// public:
//     CommandBuffer(const CommandPool &pool)
//         : pool(pool)
//     {
//         VkCommandBufferAllocateInfo ai = {
//             .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
//             .pNext = nullptr,
//             .commandPool = pool.pool,
//             .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
//             .commandBufferCount = 1,
//         };

//         VkResult result = vkAllocateCommandBuffers(pool.device, &ai, &buffer);
//         if (result != VK_SUCCESS)
//         {
//             throw std::runtime_error("Failed allocate cmd buffer");
//         }
//     }
// };

// class Swapchain
// {
//     VkSwapchainKHR swapchain;
//     const Surface& surface;

//     uint32_t getMinImageCount()
//     {
//         VkSurfaceCapabilitiesKHR caps;
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
//         return caps.minImageCount;
//     }

//     VkExtent2D getExtent()
//     {
//         VkSurfaceCapabilitiesKHR caps;
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
//         return caps.currentExtent;
//     }

//     VkExtent2D getTransform()
//     {
//         VkSurfaceCapabilitiesKHR caps;
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
//         return caps.currentTransform;
//     }

//     VkImageUsageFlags getUsage()
//     {
//         VkSurfaceCapabilitiesKHR caps;
//         vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
//         if (caps.supportedUsageFlags | VK_IMAGE_USAGE_SAMPLED_BIT &&
//             caps.supportedUsageFlags | VK_IMAGE_USAGE_STORAGE_BIT)
//             return VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
//         return caps.supportedUsageFlags | VK_IMAGE_USAGE_SAMPLED_BIT;
//     }

// public:
//     Swapchain(const Surface &surface)
//         : surface(surface)
//     {
//         VkSwapchainCreateInfoKHR ci = {
//             .sType = VK_STRUCTURE_TYPE_SWAPCHAIN,
//             .pNext = nullptr,
//             .flags = 0,
//             .surface = surface,
//             .minImageCount = 2,
//             .imageFormat = R8G8B8A8_UNORM,
//             .imageColorSpace = SRGB_NONLINEAR_KHR,
//             .imageExtent = getExtent(),
//             .imageArrayLayers = 1,
//             .imageUsage = getUsage(),
//             .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
//             .queueFamilyIndexCount = 0,
//             .pQueueFamilyIndices = nullptr,
//             .preTransform = getTransform(),
//             .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
//             .presentMode = device.GetBestPresentMode(),
//             .clipped = VK_TRUE;
//             .oldSwapchain = nullptr;
//         };

//     }
// };

int main()
{
    Engine engine = Engine();
    engine.run();

    return 0;
}