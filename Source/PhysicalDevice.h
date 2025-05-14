#pragma once

#include "Handle.h"
#include "Object.h"
#include "vulkan/vulkan.h"

class PhysicalDevice final
    : public Handle<VkPhysicalDevice>
{
    Scope &scope;

    [[nodiscard]] uint32_t getRating() const;
public:
    explicit PhysicalDevice(Scope &scope);

    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
};
