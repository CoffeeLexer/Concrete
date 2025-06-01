#define GLFW_INCLUDE_NONE

#include "Scope.h"
#include "Shader.h"
#include "ShaderManager.h"

#include <tiny_gltf.h>
#include <iostream>

bool loadModel(const char *filename, tinygltf::Model &model)
{
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }
    if (!res) {
        std::cout << "Failed to load gltf: " << filename << std::endl;
        return false;
    }
    std::cout << "Loaded gltf: " << filename << std::endl;
    return true;
}

int main()
{
    tinygltf::Model model;
    Scope s{};
    Shader error = s.getShaderManager().createShader(ShaderSources{});

    while(s.isActive())
    {
        s.pollEvents();
        s.beginFrame();
        s.draw(error);
        s.endFrame();
    }
    s.waitIdle();

    return 0;
}