#pragma once

#include "vulkan/vulkan.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"
#include "Window.h"
#include "Swapchain.h"
#include "Pipeline.h"

class Engine
{
    // NOTE: Initialization order is defined by member order
    Instance        instance;
    PhysicalDevice  physicalDevice;
    Window          window;
    Surface         surface;
    Device          device;
    Swapchain       swapchain;
    Pipeline        pipeline;

public:
    operator VkInstance&();
    operator VkPhysicalDevice&();
    operator VkDevice&();
    operator Device&();
    operator Pipeline&();
    operator VkSurfaceKHR&();
    operator Window&();
    operator Swapchain&();

    Engine();
    ~Engine();
    void run();
};
