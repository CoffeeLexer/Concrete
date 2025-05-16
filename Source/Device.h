#pragma once

#include <optional>

#include "vulkan/vulkan.h"
#include <vector>

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
    void createPhysicalDevice();
    void createLogicalDevice();
public:
    explicit Device(Scope &scope);
    ~Device();
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    const Queues& getQueues() { return queues; }
    const VkPhysicalDevice& getVkPhysicalDevice() { return physicalDevice; }
    const VkDevice& getVkDevice() { return logicalDevice; }
};
