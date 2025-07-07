#pragma once

#include <optional>

#include "vulkan/vulkan.h"
#include <vector>
#include "Traits.h"

struct Queue : DisableCopy
{
    VkQueue queue = VK_NULL_HANDLE;
    uint32_t index = 0;
};

struct Queues : DisableCopy
{
    Queue graphics = {};
    Queue present = {};
    Queue transfer = {};
    bool sameFamily() const {
        return graphics.index == present.index &&
            graphics.index == transfer.index;
    }
};

class Scope;
class Device : DisableCopy
{
    Scope &scope;

    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    Queues queues;

    Queues selectQueueFamilies();
    void createPhysicalDevice();
    void createLogicalDevice();
public:
    explicit Device(Scope &scope);
    ~Device();

    const Queues& getQueues() const { return queues; }
    const VkPhysicalDevice& getVkPhysicalDevice() const { return physicalDevice; }
    const VkDevice& getVkDevice() const { return logicalDevice; }
};
