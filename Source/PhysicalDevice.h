#pragma once

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"
#include "vulkan/vulkan.h"

class PhysicalDevice : public Handle<VkPhysicalDevice>
{
    friend Scope::Scope(), Scope::~Scope();
    ScopeLink scope;

    explicit PhysicalDevice(Scope *scope);

    void Create();
    void Destroy();

    [[nodiscard]] uint32_t getRating() const;
public:
    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
};
