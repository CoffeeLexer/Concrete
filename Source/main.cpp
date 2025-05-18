#define GLFW_INCLUDE_NONE

#include "Scope.h"

int main()
{
    Scope s{};

    while(s.isActive())
    {
        s.pollEvents();
        s.draw();
    }

    return 0;
}