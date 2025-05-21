#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class Scope;
class ShaderManager;
class Shader {
    friend ShaderManager;
    Scope &scope;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    Shader(Scope &scope, VkPipelineLayout layout, VkPipeline pipeline)
    : scope(scope)
    , layout(layout)
    , pipeline(pipeline)
    {}

public:
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    ~Shader();
//    const VkPipelineLayout getVkPipelineLayout() const { return layout; }
    const VkPipeline getVkPipeline() const { return pipeline; }
};

