#pragma once

#include "vulkan/vulkan.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"
#include "Window.h"
#include "Backbuffer.h"
#include "Pipeline.h"

class Engine
{
public:
    Window     window;
    Surface    surface;
    Instance   instance;
    PhysicalDevice physicalDevice;
    Device     device;
    Backbuffer backbuffer;
    Pipeline   pipeline;

    explicit Engine();
    ~Engine();
    void run();
};
