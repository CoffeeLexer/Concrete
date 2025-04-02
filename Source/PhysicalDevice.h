#pragma once

#include "Handle.h"
#include "Scoped.h"
#include "vulkan/vulkan.h"

class PhysicalDevice
    : public Handle<VkPhysicalDevice>
    , public Scoped
{
    friend Scope;
    PhysicalDevice() = default;
    void Create();
    void Destroy();

    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
public:

};
