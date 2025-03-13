#pragma once

#include "Handle.h"
#include "Link.h"
#include "vulkan/vulkan.h"

class Engine;

class PhysicalDevice
    : public Handle<VkPhysicalDevice>
    , public Link<Engine>
{
    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
public:
    explicit PhysicalDevice(Engine *engine);
};
