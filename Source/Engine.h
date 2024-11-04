#pragma once
#include "vulkan/vulkan.h"
class Instance;
class PhysicalDevice;
class Device;
class Surface;

class Engine
{
    const Instance &instance;
    const PhysicalDevice &physicalDevice;
    const Device &device;
    const Surface &surface;
public:
    Engine();
    ~Engine();
    void run();
};
