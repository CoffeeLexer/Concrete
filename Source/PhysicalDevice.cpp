#include "PhysicalDevice.h"

#include <limits>
#include <vector>

#include "Engine.h"

static VkPhysicalDeviceProperties StaticProperties(const VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    return properties;
}

VkPhysicalDeviceProperties PhysicalDevice::Properties() const
{
    return StaticProperties(handle);
}

static VkPhysicalDeviceFeatures StaticFeatures(const VkPhysicalDevice device)
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    return features;
}

VkPhysicalDeviceFeatures PhysicalDevice::Features() const
{
    return StaticFeatures(handle);
}

uint32_t Rating(VkPhysicalDevice physicalDevice)
{
    const auto& features = StaticFeatures(physicalDevice);
    const auto& properties = StaticProperties(physicalDevice);
    auto limit = std::numeric_limits<uint32_t>::max();

    switch(properties.deviceType)
    {
    default:
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        limit /= 2;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        limit /= 2;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        limit /= 2;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        break;
    }
    return limit;
}

void PhysicalDevice::Create()
{
    VkInstance instance = getScope().instance;
    uint32_t count;
    std::vector<VkPhysicalDevice> devices;

    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    devices.resize(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());

    std::pair best = {
        std::numeric_limits<uint32_t>::min(),
        std::numeric_limits<uint32_t>::max(),
    };
    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t rating = Rating(devices[i]);

        if (best.first < rating)
            best = { rating, i };
    }

    handle = devices[best.second];
}
