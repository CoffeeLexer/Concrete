#pragma once

#include "Handle.h"
#include "Object.h"
#include "vulkan/vulkan.h"

class PhysicalDevice final
    : public Object
    , public Handle<VkPhysicalDevice>
{
    void Create() override;
    void Destroy() override;

    [[nodiscard]] uint32_t getRating() const;
public:
    explicit PhysicalDevice(Scope &scope);

    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
};
