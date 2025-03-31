#pragma once

#include "Ownership.h"
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
    Handler<Window>         window;
    Handler<Surface>        surface;
    Handler<Instance>       instance;
    Handler<PhysicalDevice> physicalDevice;
    Handler<Device>         device;
    Handler<Backbuffer>     backbuffer;
    Handler<Pipeline>       pipeline;

    explicit Engine();
    ~Engine();
    void run();
};
