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
    Surface *surface;
    PhysicalDevice *physicalDevice;
    Device *device;
public:
    Scope();
    ~Scope();

    Instance& getInstance();
    PhysicalDevice& getPhyDevice();
    Device& getDevice();
    Window& getWindow();
    Surface& getSurface();
};