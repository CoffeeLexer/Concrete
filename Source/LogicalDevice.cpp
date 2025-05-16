#include "Device.h"

#include "Panic.h"
#include "Scope.h"

std::vector<const char*> GetExtensions()
{
    return {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
}

struct QueueFamilyProperties : public std::vector<VkQueueFamilyProperties> {
    QueueFamilyProperties(const VkPhysicalDevice& phyDevice) {
        uint32_t count;
        vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &count, nullptr);
        this->resize(count);
        vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &count, this->data());
    }
};

struct PresentSupportList : public std::vector<VkBool32> {
    PresentSupportList(const VkPhysicalDevice phyDevice, const VkSurfaceKHR surf) {
        uint32_t familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(phyDevice, &familyCount, nullptr);
        this->resize(familyCount);
        for (uint32_t i = 0; i < familyCount; i++)
            vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, i, surf, &(*this)[i]);
    }
};

namespace Support {
    enum Enum {
        None = 0,
        Graphics = 1,
        Compute = 2,
        Transfer = 4,
        Present = 8,
    };
}

Queues Device::selectQueueFamilies()
{
    auto presentSupport = PresentSupportList{physicalDevice, scope.getWindow().getVkSurface()};
    auto queueFamilyProperties = QueueFamilyProperties{physicalDevice};

    std::vector<int> candidates(presentSupport.size(), Support::None);

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

    Queues queuesCandidate{};
    // Let's prefer 1 queue over 2
    for (uint32_t i = 0; i < candidates.size(); i++)
    {
        auto all = (Support::Enum)(Support::Present | Support::Graphics);
        auto candidate = candidates[i];
        if ((candidate & all) == all) {
            queuesCandidate.graphics.index = i;
            queuesCandidate.present.index = i;
            return queuesCandidate;
        }
    }

    std::optional<uint32_t> presentIndex, graphicsIndex;
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

    queuesCandidate.present.index = presentIndex.value();
    queuesCandidate.graphics.index = graphicsIndex.value();
    return queuesCandidate;
}

void Device::createLogicalDevice()
{
    constexpr float priorities[] = {1.0f, 1.0f};
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};

    if (queues.sameFamily())
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queues.graphics.index,
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
            .queueFamilyIndex = queues.graphics.index,
            .queueCount = 1,
            .pQueuePriorities = priorities,
        };
        VkDeviceQueueCreateInfo presentQueueInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queues.present.index,
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

    VkResult result = vkCreateDevice(physicalDevice, &ci, nullptr, &logicalDevice);
    if (result != VK_SUCCESS)
        panic("Failed Device Creation");

    vkGetDeviceQueue(logicalDevice, queues.graphics.index, 0, &queues.graphics.queue);
    const uint32_t presentQueueIndex = queues.sameFamily() ? 0 : 1;
    vkGetDeviceQueue(logicalDevice, queues.present.index, presentQueueIndex, &queues.present.queue);
}
