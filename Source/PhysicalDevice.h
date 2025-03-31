#pragma once

#include "Handle.h"
#include "Ownership.h"
#include "vulkan/vulkan.h"

class Engine;

class PhysicalDevice
    : public Handle<VkPhysicalDevice>
    , public Owned
{
    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
public:
    explicit PhysicalDevice();
};
