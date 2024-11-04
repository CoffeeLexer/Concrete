#include "Surface.h"

Surface::Surface(const Engine &instance, const VkSurfaceKHR &surface)
    : engine(engine)
    , surface(surface) 
{
}

Surface::~Surface()
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}
