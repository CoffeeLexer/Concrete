#include "Engine.h"

#include "Instance.h"

Engine::Engine()
    : instance(Instance(this))
    , physicalDevice(PhysicalDevice(this))
    , window(Window(this))
{

}

void Engine::run()
{
    const Window &window = engine;
    while (window.IsValid())
    {
        window.PollEvents();
        window.SwapBuffers();
    }
}