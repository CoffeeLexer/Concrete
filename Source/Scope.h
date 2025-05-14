#pragma once

#include "Handle.h"

class Instance;
class Window;
class Device;

class Scope
{
    Instance *instance;
    Window *window;
    Device *device;
public:
    Scope();
    ~Scope();

    Handle<Instance> getInstance{*instance};
    Handle<Window> getWindow{*window};
    Handle<Device> getDevice{*device};
};