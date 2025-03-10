#pragma once

#include "Handle.h"
#include "vulkan/vulkan.h"

class PhysicalDevice : public Handle<VkPhysicalDevice>
{
    [[nodiscard]] uint32_t Rating() const;
    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
    [[nodiscard]] static VkPhysicalDevice FindBest(VkInstance instance);

protected:
    PhysicalDevice() = default;
    void Create(VkInstance instance);
};
