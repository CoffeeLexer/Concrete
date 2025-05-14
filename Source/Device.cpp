#include "Device.h"

#include "Engine.h"
#include "Panic.h"

#include <vector>



Device::Device(Scope &scope) : scope(scope)
{
    physicalDevice = createPhysicalDevice();
    selectQueueFamilies();
    logicalDevice = createLogicalDevice();
}

Device::~Device()
{
    vkDestroyDevice(logicalDevice, nullptr);
}
