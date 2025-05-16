#include "Device.h"
#include "Scope.h"
#include <vector>

struct Properties : public VkPhysicalDeviceProperties {
    Properties(const VkPhysicalDevice &phyDev) {
        vkGetPhysicalDeviceProperties(phyDev, this);
    }
};

std::vector<VkPhysicalDevice> physicalDeviceVector(const VkInstance instance) {
    uint32_t count;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> physicalDevice{count};
    vkEnumeratePhysicalDevices(instance, &count, physicalDevice.data());
    return physicalDevice;
}

uint32_t rate(const VkPhysicalDevice physicalDevice) {
    const auto properties = Properties{physicalDevice};
    switch(properties.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return 1 << 31;
    default:
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return 1 << 30;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return 1 << 29;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return 1 << 28;
    }
}


void Device::createPhysicalDevice()
{
    const auto instance = scope.getInstance().getVkInstance();
    const auto devices = physicalDeviceVector(instance);

    uint32_t bestRating;
    VkPhysicalDevice bestDevice;
    for (auto phyDevice : devices)
    {
        uint32_t rating = rate(phyDevice);
        if (bestRating < rating)
        {
            bestRating = rating;
            bestDevice = phyDevice;
        }
    }
    physicalDevice = bestDevice;
}
