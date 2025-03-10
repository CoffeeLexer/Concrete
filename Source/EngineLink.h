#pragma once
#include <Engine.h>
#include <stdexcept>

class Engine;

class EngineLink
{
    Engine *engine;

    Engine& deref() const
    {
        if (engine == nullptr)
            throw std::runtime_error("EngineLink::Deref getting NULL pointer");
        return *engine;
    }

public:
    explicit EngineLink(Engine *engine)
        : engine(engine)
    {}
    const Device& Device()
    {
        const auto& device = deref().device;
        if (device == nullptr)
            throw std::runtime_error("EngineLink::Device is NULL");
        return *device;
    }
    const Window& Window()
    {
        const auto& window = deref().window;
        if (window == nullptr)
            throw std::runtime_error("EngineLink::Window is NULL");
        return *window;
    }
    const Backbuffer& Backbuffer()
    {
        const auto& backbuffer = deref().backbuffer;
        if (backbuffer == nullptr)
            throw std::runtime_error("EngineLink::Backbuffer is NULL");
        return *backbuffer;
    }
};