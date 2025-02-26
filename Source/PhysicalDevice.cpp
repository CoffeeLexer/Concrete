#include "PhysicalDevice.h"

#include <limits>
#include <vector>

#include "Engine.h"

PhysicalDevice::PhysicalDevice(Engine& engine, VkPhysicalDevice physicalDevice)
    : engine(engine)
    , physicalDevice(physicalDevice)
{

}

VkPhysicalDeviceProperties PhysicalDevice::Properties() const
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    return properties;
}

VkPhysicalDeviceFeatures PhysicalDevice::Features() const
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
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

PhysicalDevice::operator VkPhysicalDevice&()
{
    return physicalDevice;
}

PhysicalDevice PhysicalDevice::FindBest(Engine& engine)
{
    const VkInstance &instance = engine;
    uint32_t count;
    std::vector<VkPhysicalDevice> devices;

    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    devices.resize(count);

    vkEnumeratePhysicalDevices(instance, &count, devices.data());

    for (uint32_t i = 0; i < count; i++)
    {
        auto device = PhysicalDevice(engine, devices[i]);

    }

    return PhysicalDevice(engine, devices[i]);
}

