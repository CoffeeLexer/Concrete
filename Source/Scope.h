#pragma once

class Instance;
class PhysicalDevice;
class Device;
class Window;
class Surface;

class Scope
{
    Instance *instance;
    Window *window;
    Device *device;
public:
    Scope();

    ~Scope()
    {
        device->Destroy();
        window->Destroy();
        delete device;
        delete window;
    }
};