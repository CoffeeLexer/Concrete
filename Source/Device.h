#pragma once
#include "vulkan/vulkan.h"
class Engine;

class Device
{
    const Engine& engine;

    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uint32_t graphicsIndex;
    uint32_t presentIndex;

    std::vector<VkBool32> GetPresentSupportVector()
    {
        uint32_t familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
        std::vector<VkBool32> supportArray;
        supportArray.resize(familyCount);

        for (uint32_t i = 0; i < familyCount; i++)
        {
            VkBool32 supported;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported);
            supportArray[i] = supported;
        }
        return supportArray;
    }

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties()
    {
        uint32_t familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> properties;
        properties.resize(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, properties.data());
        return properties;
    }

    std::tuple<uint32_t, uint32_t> PickQueueFamily()
    {
        auto presentSupport = GetPresentSupportVector();
        auto queueFamilyProperties = GetQueueFamilyProperties();

        for (uint32_t i = 0; i < presentSupport.size(); i++)
        {
            if (presentSupport[i] == VK_TRUE)
            {
                const auto& property = queueFamilyProperties.at(i);
                if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    return {i, i};
            }
        }

        uint32_t graphicsQueue = UINT32_MAX;
        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            const auto& property = queueFamilyProperties.at(i);

            if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueue = i;
                break;
            }
        }

        if (graphicsQueue == UINT32_MAX)
            throw std::runtime_error("No queue graphics family");

        uint32_t presentQueue = UINT32_MAX;
        for (uint32_t i = 0; i < presentSupport.size(); i++)
        {
            if (presentSupport.at(i) == VK_TRUE)
            {
                presentQueue = i;
                break;
            }
        }

        if (presentQueue == UINT32_MAX)
            throw std::runtime_error("No queue present family");

        return {presentQueue, graphicsQueue};
    }

    std::vector<const char*> GetExtensions()
    {
        return (std::vector<const char*>) {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
    }
public:
    operator const VkDevice&() const
    {
        return device;
    }

    operator const VkPhysicalDevice&() const
    {
        return physicalDevice;
    }

    // TODO: Queue Factory or mngr
    Device(const Instance& i, const Surface& s);

    VkPresentModeKHR GetBestPresentMode();

    ~Device();
};