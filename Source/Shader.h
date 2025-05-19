#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class Scope;
class Shader;
class ShaderManager
{
    Scope &scope;

public:
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
    ShaderManager(Scope &scope) : scope(scope) {};
    Shader createShader(std::string vertex, std::string fragment);
};

class Shader {
    friend ShaderManager;
    Scope &scope;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    Shader(Scope &scope, ) : scope(scope) {}
public:
};

