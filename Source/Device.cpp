#include "Device.h"

#include <set>

#include "Engine.h"
#include "Panic.h"

#include <vector>

std::vector<const char*> GetExtensions()
{
    return {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
}

std::vector<VkQueueFamilyProperties> Device::GetQueueFamilyProperties()
{
    uint32_t count;
    VkPhysicalDevice phyDev = scope().getPhyDevice().getHandle();
    vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties;
    properties.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &count, properties.data());
    return properties;
}

std::vector<VkBool32> Device::GetPresentSupportVector()
{
    uint32_t familyCount;
    VkPhysicalDevice physicalDevice = scope().getPhyDevice().getHandle();
    VkSurfaceKHR surface = scope().getSurface().getHandle();
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

void Device::Create()
{
    VkPhysicalDevice physicalDevice = scope().getPhyDevice().getHandle();

    float priorities[] = {1.0f, 1.0f};
    const auto & [presentFamily, graphicsFamily] = PickQueueFamily();
    graphicsIndex = graphicsFamily;
    presentIndex = presentFamily;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};

    if (presentFamily == graphicsFamily)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = presentFamily,
            .queueCount = 2,
            .pQueuePriorities = priorities,
        };
        queueCreateInfos.push_back(queueCreateInfo);
    }
    else
    {
        VkDeviceQueueCreateInfo graphicsQueueInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = graphicsFamily,
            .queueCount = 1,
            .pQueuePriorities = priorities,
        };
        VkDeviceQueueCreateInfo presentQueueInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = presentFamily,
            .queueCount = 1,
            .pQueuePriorities = priorities,
        };
        queueCreateInfos.push_back(presentQueueInfo);
    }

    auto extensions = GetExtensions();
    VkDeviceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    VkResult result = vkCreateDevice(physicalDevice, &ci, nullptr, &handle);
    if (result != VK_SUCCESS)
        panic("Failed Device Creation");

    vkGetDeviceQueue(handle, graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(handle, presentFamily, 0, &presentQueue);
}

void Device::Destroy()
{
    vkDestroyDevice(handle, nullptr);
}

std::tuple<uint32_t, uint32_t> Device::PickQueueFamily()
{
    auto presentSupport = GetPresentSupportVector();
    auto queueFamilyProperties = GetQueueFamilyProperties();

    std::vector<uint32_t> candidates(presentSupport.size(), 0);
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
        panic("No queue graphics family");

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
        panic("No queue present family");

    return {presentQueue, graphicsQueue};
}

uint32_t Device::GetGraphicsIndex() const
{
    return graphicsIndex;
}

uint32_t Device::GetPresentIndex() const
{
    return presentIndex;
}
VkQueue Device::GetGraphicsQueue() const
{
    return graphicsQueue;
}
VkQueue Device::GetPresentQueue() const
{
    return presentQueue;
}
