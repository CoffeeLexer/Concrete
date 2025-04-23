#include "PhysicalDevice.h"

#include <limits>
#include <vector>

#include "Engine.h"

PhysicalDevice::PhysicalDevice(Scope &scope) : Object(scope) {}

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

uint32_t PhysicalDevice::getRating() const
{
    const auto& properties = Properties();
    const uint32_t limit = std::numeric_limits<uint32_t>::max();

    switch(properties.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return limit;
    default:
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return limit / 2;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return limit / 4;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return limit / 8;
    }
}

void PhysicalDevice::Create() override
{
    const VkInstance &instance = scope().getInstance().getHandle();
    uint32_t count;
    std::vector<VkPhysicalDevice> devices;

    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    devices.resize(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());

    uint32_t bestRating;
    VkPhysicalDevice bestDevice;
    for (uint32_t i = 0; i < count; i++)
    {
        handle = devices[i];
        uint32_t rating = getRating();

        if (bestRating < rating)
        {
            bestRating = rating;
            bestDevice = handle;
        }
    }
    handle = bestDevice;
}

void PhysicalDevice::Destroy() override {}
