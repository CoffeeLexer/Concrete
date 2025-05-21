#include "Shader.h"

#include "Scope.h"

Shader::~Shader()
{
    VkDevice device = scope.getDevice().getVkDevice();
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, layout, nullptr);
}