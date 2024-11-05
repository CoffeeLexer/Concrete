#include "Surface.h"

Surface::Surface(Engine &engine)
    : engine(engine)
{
}

Surface::~Surface()
{
    VkInstance &instance = engine;
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

Surface::operator VkSurfaceKHR&()
{
    return surface;
}
