#pragma once

#include "vulkan/vulkan.h"

class Engine;

class PhysicalDevice
{
    Engine& engine;
    VkPhysicalDevice physicalDevice;

    VkPhysicalDevice pickPhysicalDevice();

    VkPhysicalDeviceFeatures Features() const;
    uint32_t Rating() const;
    VkPhysicalDeviceProperties Properties() const;

    PhysicalDevice(Engine& engine, VkPhysicalDevice physicalDevice);
public:
    operator VkPhysicalDevice&();

    static PhysicalDevice FindBest(Engine &engine);
};
