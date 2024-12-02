#pragma once

#include <vulkan/vulkan.h>

class Engine;

class PipelineLayout
{
    VkPipelineLayout pipelineLayout;
    Engine &engine;
public:
    PipelineLayout(Engine &engine);
    ~PipelineLayout(); 
};