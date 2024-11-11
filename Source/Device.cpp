#include "Device.h"

#include "Engine.h"

#include <vector>
#include <stdexcept>

std::vector<VkQueueFamilyProperties> Device::GetQueueFamilyProperties()
{
    uint32_t familyCount;
    VkPhysicalDevice &physicalDevice = engine;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> properties;
    properties.resize(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, properties.data());
    return properties;
}

std::vector<VkBool32> Device::GetPresentSupportVector()
{
    uint32_t familyCount;
    VkPhysicalDevice &physicalDevice = engine;
    VkSurfaceKHR &surface = engine;
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

Device::Device(Engine &engine)
    : engine(engine)
{
    VkPhysicalDevice &physicalDevice = engine;

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

    VkResult result = vkCreateDevice(physicalDevice, &ci, nullptr, &device);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed device create");
    }
    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
}

VkPresentModeKHR Device::GetBestPresentMode()
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


Device::~Device()
{
    vkDestroyDevice(device, nullptr);
}

std::vector<const char*> Device::GetExtensions()
{
    return (std::vector<const char*>) {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
}

std::tuple<uint32_t, uint32_t> Device::PickQueueFamily()
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

Device::operator VkDevice&()
{
    return device;
}

uint32_t Device::GetGraphicsIndex()
{
    return graphicsIndex;
}

uint32_t Device::GetPresentIndex()
{
    return presentIndex;
}
