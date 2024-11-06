#pragma once

#include "vulkan/vulkan.h"

class Engine;

class PhysicalDevice
{
    Engine& engine;
    VkPhysicalDevice physicalDevice;

    VkPhysicalDevice pickPhysicalDevice();
public:
    operator VkPhysicalDevice&();

    PhysicalDevice(Engine& engine);
    ~PhysicalDevice();
};
