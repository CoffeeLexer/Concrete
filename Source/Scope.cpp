#include "Scope.h"

#include "Window.h"
#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"
#include "Surface.h"

Scope::Scope()
{
    instance = new Instance(*this);
    physicalDevice = new PhysicalDevice(*this);
    window = new Window(*this);
    surface = new Surface(*this);
    device = new Device(*this);
}

Scope::~Scope()
{
    delete device;
    delete surface;
    delete window;
    delete physicalDevice;
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
