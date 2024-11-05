#pragma once

#include "vulkan/vulkan.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"
#include "Window.h"

class Engine
{
    Instance        instance;
    PhysicalDevice  physicalDevice;
    Device          device;
    Surface         surface;
    Window          window;

public:
    operator VkInstance&();
    operator VkPhysicalDevice&();
    operator VkDevice&();
    operator VkSurfaceKHR&();

    Engine();
    ~Engine();
    void run();
};
