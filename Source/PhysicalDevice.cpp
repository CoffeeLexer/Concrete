#include "Device.h"

#include <limits>
#include <vector>

#include "Engine.h"

struct Properties : public VkPhysicalDeviceProperties {
    Properties(const VkPhysicalDevice &phyDev) {
        vkGetPhysicalDeviceProperties(phyDev, this);
    }
};

struct Features : public VkPhysicalDeviceFeatures {
    Features(const VkPhysicalDevice &phyDev) {
        vkGetPhysicalDeviceFeatures(phyDev, this);
    }
};

struct PhysicalDeviceVector : public std::vector<VkPhysicalDevice> {
    PhysicalDeviceVector(const VkInstance &instance) {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        this->resize(count);
        vkEnumeratePhysicalDevices(instance, &count, this->data());
    }
};

uint32_t getRating(const VkPhysicalDevice &phyDevice) const
{
    const auto& properties = Properties{phyDevice};
    constexpr uint32_t limit = std::numeric_limits<uint32_t>::max();

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

VkPhysicalDevice Device::createPhysicalDevice() const
{
    const VkInstance &instance = scope.getInstance().getHandle();
    const auto devices = PhysicalDeviceVector{instance};

    uint32_t bestRating;
    VkPhysicalDevice bestDevice;
    for (auto phyDevice : devices)
    {
        if (uint32_t rating = getRating(phyDevice); bestRating < rating)
        {
            bestRating = rating;
            bestDevice = phyDevice;
        }
    }
    return bestDevice;
}
