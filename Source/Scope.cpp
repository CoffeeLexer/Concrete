#include "Scope.h"

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Device.h"

Scope::Scope()
{
    instance = new Instance(this);
    instance->Create();
    physicalDevice = new PhysicalDevice(this);
    physicalDevice->Create();
    device = new Device(this);
    device->Create();
}

Scope::~Scope()
{
    instance->Destroy();
    delete instance;
}
