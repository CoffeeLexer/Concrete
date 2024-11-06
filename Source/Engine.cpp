#include "Engine.h"

Engine::Engine()
    : instance(Instance(*this))
    , physicalDevice(PhysicalDevice(*this))
    , window(Window(*this))
    , surface(Surface(*this))
    , device(Device(*this))
{

}

Engine::~Engine()
{
}

void Engine::run()
{
    while (window.IsValid())
    {
        window.PollEvents();
        window.SwapBuffers();
    }
}

Engine::operator VkInstance&()
{
    return instance;
}
Engine::operator VkPhysicalDevice&()
{
    return physicalDevice;
}
Engine::operator VkDevice&()
{
    return device;
}
Engine::operator VkSurfaceKHR&()
{
    return surface;
}