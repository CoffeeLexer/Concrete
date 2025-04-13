#include "Scope.h"

#include "Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"

Scope::Scope()
{
    instance = new Instance(this);
    instance->Create();
    physicalDevice = new PhysicalDevice(this);
    physicalDevice->Create();
    window = new Window(this);
    window->Create();
    surface = new Surface(this);
    surface->Create();
    device = new Device(this);
    device->Create();
}

Scope::~Scope()
{
    device->Destroy();
    delete device;
    surface->Destroy();
    delete surface;
    window->Destroy();
    delete window;
    physicalDevice->Destroy();
    delete physicalDevice;
    instance->Destroy();
    delete instance;
}

Instance& Scope::getInstance()
{
    return *instance;
}

PhysicalDevice& Scope::getPhyDevice()
{
    return *physicalDevice;
}

Device &Scope::getDevice()
{
    return *device;
}

Surface &Scope::getSurface()
{
    return *surface;
}

Window &Scope::getWindow()
{
    return *window;
}
