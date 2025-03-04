#pragma once

#include "vulkan/vulkan.h"
#include <vector>

class Engine;

class Device
{
    Engine& engine;

    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uint32_t graphicsIndex;
    uint32_t presentIndex;

    std::vector<VkBool32> GetPresentSupportVector();

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    std::tuple<uint32_t, uint32_t> PickQueueFamily();
    std::vector<const char*> GetExtensions();
public:
    operator VkDevice&();

    Device(Engine &engine);
    ~Device();

    uint32_t GetGraphicsIndex() const;
    uint32_t GetPresentIndex() const;

    VkQueue GetGraphicsQueue() const;
    VkQueue GetPresentQueue() const;
};