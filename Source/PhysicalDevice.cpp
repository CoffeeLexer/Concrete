#include "Device.h"
#include "Scope.h"
#include <vector>

struct Properties : public VkPhysicalDeviceProperties {
    Properties(const VkPhysicalDevice &phyDev) {
        vkGetPhysicalDeviceProperties(phyDev, this);
    }
};

// struct Features : public VkPhysicalDeviceFeatures {
//     Features(const VkPhysicalDevice &phyDev) {
//         vkGetPhysicalDeviceFeatures(phyDev, this);
//     }
// };

struct PhysicalDeviceVector : public std::vector<VkPhysicalDevice> {
    PhysicalDeviceVector(const VkInstance &instance) {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        this->resize(count);
        vkEnumeratePhysicalDevices(instance, &count, this->data());
    }
};

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


VkPhysicalDevice Device::createPhysicalDevice()
{
    const auto instance = scope.getInstance().getVkInstance();
    const auto devices = PhysicalDeviceVector{instance};

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
    return bestDevice;
}
