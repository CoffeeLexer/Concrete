#include "PhysicalDevice.h"

#include <limits>
#include <vector>

#include "Engine.h"

void PhysicalDevice::Create(VkInstance instance)
{
    handle = FindBest(instance);
}

VkPhysicalDeviceProperties PhysicalDevice::Properties() const
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(handle, &properties);
    return properties;
}

VkPhysicalDeviceFeatures PhysicalDevice::Features() const
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(handle, &features);
    return features;
}

uint32_t PhysicalDevice::Rating() const
{
    const auto& features = Features();
    const auto& properties = Properties();
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

VkPhysicalDevice PhysicalDevice::FindBest(VkInstance instance)
{
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
        auto device = PhysicalDevice();
        device.handle = devices[i];

        if (best.first < device.Rating())
            best = { device.Rating(), i };
    }

    return devices[best.second];
}


