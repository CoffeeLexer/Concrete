#pragma once

#include <vulkan/vulkan.h>

class Engine;

class DescriptorSetLayout
{
    Engine &engine;
    VkDescriptorSetLayout setLayout;
public:
    DescriptorSetLayout(Engine &engine);
};
