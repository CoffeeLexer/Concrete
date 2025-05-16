#include "Device.h"

#include "Scope.h"

Device::Device(Scope &scope) : scope(scope)
{
    createPhysicalDevice();
    selectQueueFamilies();
    createLogicalDevice();
}

Device::~Device()
{
    vkDestroyDevice(logicalDevice, nullptr);
}
