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

uint32_t Device::GetGraphicsIndex() const
{
    return graphicsIndex.value();
}

uint32_t Device::GetPresentIndex() const
{
    return presentIndex.value();
}
VkQueue Device::GetGraphicsQueue() const
{
    return graphicsQueue;
}
VkQueue Device::GetPresentQueue() const
{
    return presentQueue;
}
