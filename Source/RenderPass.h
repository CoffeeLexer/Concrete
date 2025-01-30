#pragma once

#include "vulkan/vulkan.h"

class Engine;

class RenderPass
{
    Engine &engine;
    VkRenderPass renderPass;

public:
    RenderPass(Engine &engine);
    ~RenderPass();
    operator VkRenderPass();
};
