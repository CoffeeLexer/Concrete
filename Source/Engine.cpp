#include "Engine.h"

#include <stdexcept>

Engine::Engine()
    : instance(this)
    , physicalDevice(this)
    , device(this)
    , window(this)
    , surface(this)
    , backbuffer(this)
    , pipeline(this)
{
}
Engine::~Engine() = default;

void Engine::run()
{
    while (window.IsValid())
    {
        backbuffer.Draw();
        window.PollEvents();
    }
}
