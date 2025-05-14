#pragma once

#include <optional>

#include "vulkan/vulkan.h"
#include <vector>

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"

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

class Device
{
    Scope &scope;

    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    Queues selectQueueFamilies();
    VkPhysicalDevice createPhysicalDevice();
    VkDevice createLogicalDevice();
public:
    explicit Device(Scope &scope);
    ~Device();

    Queues queues;

    [[nodiscard]] uint32_t GetGraphicsIndex() const;
    [[nodiscard]] uint32_t GetPresentIndex() const;

    [[nodiscard]] VkQueue GetGraphicsQueue() const;
    [[nodiscard]] VkQueue GetPresentQueue() const;
};
