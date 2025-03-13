#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include "Handle.h"
#include "Link.h"

class Engine;

class Pipeline
    : public Handle<VkPipeline>
    , public Link<Engine>
{
    VkPipelineLayout pipelineLayout;

    VkShaderModule CreateShaderModule(std::vector<uint32_t> code);
public:
    explicit Pipeline(Engine* engine);
    ~Pipeline();
};
