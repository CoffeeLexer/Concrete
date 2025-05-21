#define GLFW_INCLUDE_NONE

#include "Scope.h"
#include "Shader.h"
#include "ShaderManager.h"

int main()
{
    Scope s{};

    ShaderSources emptySources{};
    Shader error = s.getShaderManager().createShader(emptySources);

    while(s.isActive())
    {
        s.pollEvents();
        s.beginFrame();
        s.draw(error);
        s.endFrame();
    }

    return 0;
}