#pragma once

#include <vulkan/vulkan.h>

class Engine;

class DescriptorPool
{
    Engine &engine;
    VkDescriptorPool descriptionPool;
public:
    DescriptorPool(Engine &engine);
};
