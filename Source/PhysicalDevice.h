#pragma once

#include "Engine.h"

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
