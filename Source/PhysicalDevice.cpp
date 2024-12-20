#include "PhysicalDevice.h"

#include <vector>

#include "Engine.h"

PhysicalDevice::PhysicalDevice(Engine& engine)
    : engine(engine)
    , physicalDevice(pickPhysicalDevice())
{

}

PhysicalDevice::~PhysicalDevice()
{
    
}

VkPhysicalDevice PhysicalDevice::pickPhysicalDevice()
{
    uint32_t count;
    const VkInstance& instance = engine;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> devices = {};
    devices.resize(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    if (count == 1)
        return devices.at(0);

    for (const auto& device : devices)
    {
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            return device;
    }

    return devices.at(0);
}

PhysicalDevice::operator VkPhysicalDevice&()
{
    return physicalDevice;
}
