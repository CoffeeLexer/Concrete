#include "Engine.h"

#include <stdexcept>

Engine::Engine()
    : instance(*this)
    , physicalDevice(*this)
    , device(*this)
    , window(*this)
    , surface(*this)
    , backbuffer(*this)
    , pipeline(*this)
{
    instance.Create();
    physicalDevice.Create();
    device.Create();
    window.Create();
    surface.Create();
    backbuffer.Create();
    pipeline.Create();
}
Engine::~Engine()
{
    pipeline.Destroy();
    backbuffer.Destroy();
    surface.Destroy();
    window.Destroy();
    device.Destroy();
    physicalDevice.Destroy();
    instance.Destroy();
}

void Engine::run()
{
    while (window.IsValid())
    {
        backbuffer.Draw();
        window.PollEvents();
    }
}
