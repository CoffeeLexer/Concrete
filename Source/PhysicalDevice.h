#pragma once

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"
#include "vulkan/vulkan.h"

class PhysicalDevice : public Handle<VkPhysicalDevice>
{
    friend Scope::Scope(), Scope::~Scope();
    ScopeLink scope;
    PhysicalDevice(Scope *scope);
    void Create();
    void Destroy();
public:
    [[nodiscard]] VkPhysicalDeviceFeatures Features() const;
    [[nodiscard]] VkPhysicalDeviceProperties Properties() const;
};
