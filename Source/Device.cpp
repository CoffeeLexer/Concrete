#include "Device.h"

#include "Engine.h"
#include "Panic.h"

#include <vector>

std::vector<const char*> GetExtensions()
{
    return {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
}

Device::Device(Scope *scope) : scope(scope) {}

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
    VkPhysicalDevice phyDev = scope().getPhyDevice().getHandle();
    VkSurfaceKHR surf = scope().getSurface().getHandle();
    vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &familyCount, nullptr);
    std::vector<VkBool32> supportArray;
    supportArray.resize(familyCount);

    for (uint32_t i = 0; i < familyCount; i++)
        vkGetPhysicalDeviceSurfaceSupportKHR(phyDev, i, surf, &supportArray[i]);

    return supportArray;
}

void Device::Create()
{
    VkPhysicalDevice physicalDevice = scope().getPhyDevice().getHandle();
    selectQueueFamilies();

    float priorities[] = {1.0f, 1.0f};

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};

    if (graphicsIndex == presentIndex)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = presentIndex.value(),
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
            .queueFamilyIndex = graphicsIndex.value(),
            .queueCount = 1,
            .pQueuePriorities = priorities,
        };
        VkDeviceQueueCreateInfo presentQueueInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = presentIndex.value(),
            .queueCount = 1,
            .pQueuePriorities = priorities,
        };
        queueCreateInfos.push_back(graphicsQueueInfo);
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

    vkGetDeviceQueue(handle, graphicsIndex.value(), 0, &graphicsQueue);
    const uint32_t presentQueueIndex = graphicsIndex == presentIndex ? 0 : 1;
    vkGetDeviceQueue(handle, presentIndex.value(), presentQueueIndex, &presentQueue);
}

void Device::Destroy()
{
    vkDestroyDevice(handle, nullptr);
}

enum class Support : uint32_t {
    None = 0,
    Graphics = 1,
    Compute = 2,
    Transfer = 4,
    Present = 8,
};

constexpr Support operator|(Support lhs, Support rhs)
{
    return static_cast<Support>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr void operator|=(Support& lhs, Support rhs)
{
    lhs = lhs | rhs;
}

constexpr Support operator&(Support lhs, Support rhs)
{
    return static_cast<Support>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

void Device::selectQueueFamilies()
{
    auto presentSupport = GetPresentSupportVector();
    auto queueFamilyProperties = GetQueueFamilyProperties();

    std::vector candidates(presentSupport.size(), Support::None);

    for (uint32_t i = 0; i < presentSupport.size(); i++)
    {
        if (presentSupport[i] == VK_TRUE)
            candidates[i] |= Support::Present;

        const auto& property = queueFamilyProperties.at(i);
        if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            candidates[i] |= Support::Graphics;

        if (property.queueFlags & VK_QUEUE_COMPUTE_BIT)
            candidates[i] |= Support::Compute;

        if (property.queueFlags & VK_QUEUE_TRANSFER_BIT)
            candidates[i] |= Support::Transfer;
    }

    // Let's prefer 1 queue over 2
    for (uint32_t i = 0; i < candidates.size(); i++)
    {
        Support all = Support::Present | Support::Graphics;
        auto candidate = candidates[i];
        if ((candidate & all) == all) {
            graphicsIndex = i;
            presentIndex = i;
            return;
        }
    }

    for (uint32_t i = 0; i < candidates.size(); i++)
    {
        const auto& candidate = candidates[i];
        if (!presentIndex.has_value() &&
            (candidate & Support::Present) == Support::Present)
        {
            presentIndex = i;
        }
        if (!graphicsIndex.has_value() &&
            (candidate & Support::Graphics) == Support::Graphics)
        {
            graphicsIndex = i;
        }
    }
    if (!graphicsIndex.has_value() || !presentIndex.has_value())
        panic("Queue family support missing minimal requirement");
}

uint32_t Device::GetGraphicsIndex() const
{
    return graphicsIndex.value();
}

uint32_t Device::GetPresentIndex() const
{
    return presentIndex.value();
}
VkQueue Device::GetGraphicsQueue() const
{
    return graphicsQueue;
}
VkQueue Device::GetPresentQueue() const
{
    return presentQueue;
}
