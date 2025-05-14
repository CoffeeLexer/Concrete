#pragma once

#include <optional>

#include "vulkan/vulkan.h"
#include <vector>

#include "Handle.h"

struct Queue
{
    VkQueue queue = VK_NULL_HANDLE;
    uint32_t index = 0;
};

struct Queues
{
    Queue graphics = {};
    Queue present = {};
    bool sameFamily() const {
        return graphics.index == present.index;
    }
};

class Scope;
class Device
{
    Scope &scope;

    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    Queues queues;

    Queues selectQueueFamilies();
    VkPhysicalDevice createPhysicalDevice();
    VkDevice createLogicalDevice();
public:
    explicit Device(Scope &scope);
    ~Device();

    Handle<Queues> getQueues{queues};
    Handle<VkPhysicalDevice> getVkPhysicalDevice{physicalDevice};
    Handle<VkDevice> getVkDevice{logicalDevice};
};
