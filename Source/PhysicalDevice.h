#pragma once

class PhysicalDevice
{
    const Engine& engine;
    const VkPhysicalDevice& physicalDevice;

    VkPhysicalDevice pickPhysicalDevice();
public:
    operator const VkPhysicalDevice&() const
    {
        return physicalDevice;
    }
    PhysicalDevice(const Engine& engine);
    ~PhysicalDevice();
};
