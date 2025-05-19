#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class Scope;
class Shader {
    friend ShaderManager;
    Scope &scope;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    Shader(Scope &scope, ) : scope(scope) {}
public:
};

