#pragma once

#include "vulkan/vulkan.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"
#include "Window.h"
#include "Swapchain.h"

class Engine
{
    // NOTE: Initialization order is defined by member order
    Instance        instance;
    PhysicalDevice  physicalDevice;
    Window          window;
    Surface         surface;
    Device          device;
    Swapchain       swapchain;

public:
    operator VkInstance&();
    operator VkPhysicalDevice&();
    operator VkDevice&();
    operator Device&();
    operator VkSurfaceKHR&();
    operator Window&();

    Engine();
    ~Engine();
    void run();
};
