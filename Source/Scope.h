#pragma once

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

    Instance& getInstance() { return instance; }
    Window& getWindow() { return window; }
    Device& getDevice() { return device; }
    Backbuffer& getBackbuffer() { return backbuffer; }
};