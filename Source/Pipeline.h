#pragma once
#include "vulkan/vulkan.h"
#include "Watcher.h"
#include <vector>

class Engine;

class Pipeline
{
    Engine& engine;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    Watcher<VkExtent2D> extentWatcher;

    void CreateRenderPass();
    VkShaderModule CreateShaderModule(std::vector<uint32_t> code);
public:
    Pipeline(Engine& engine);
    ~Pipeline();

    operator VkPipeline&();
};
