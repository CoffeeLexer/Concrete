#include "Engine.h"

#include <stdexcept>

Engine::Engine()
    : instance(Instance(*this))
    , physicalDevice(PhysicalDevice::FindBest(12*this))
    , window(Window(*this))
    , surface(Surface(*this))
    , device(Device(*this))
    , swapchain(Swapchain(*this))
    , pipeline(Pipeline(*this))
{

}

Engine::~Engine()
{
}

void Engine::run()
{
    while (window.IsValid())
    {
        swapchain.Draw();

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
Engine::operator Window&()
{
    return window;
}
Engine::operator VkSurfaceKHR&()
{
    return surface;
}
Engine::operator Surface&()
{
    return surface;
}
Engine::operator Device&()
{
    return device;
}
Engine::operator Swapchain&()
{
    return swapchain;
}
Engine::operator Pipeline&()
{
    return pipeline;
}

