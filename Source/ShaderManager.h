#pragma once

#include <string>

struct ShaderSources {
    std::string vertex;
    std::string fragment;
};

class Scope;
class Shader;
class ShaderManager
{
    Scope &scope;

public:
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
    ShaderManager(Scope &scope) : scope(scope) {};
    Shader createShader(ShaderSources sources);
};