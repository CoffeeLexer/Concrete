#pragma once

#include "Handle.h"
#include "Instance.h"
#include "Window.h"
#include "Device.h"
#include "Backbuffer.h"

class Scope
{
    Instance instance;
    Window window;
    Device device;
    Backbuffer backbuffer;
public:
    Scope();
    ~Scope();

    Handle<Instance> getInstance{instance};
    Handle<Window> getWindow{window};
    Handle<Device> getDevice{device};
    Handle<Backbuffer> getBackbuffer{backbuffer};
};