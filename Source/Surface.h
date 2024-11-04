#pragma once

class Surface
{
    const Engine &engine;
    const VkSurfaceKHR &surface;
public:
    operator const VkSurfaceKHR&() const
    {
        return surface;
    }
    Surface(const Engine &engine);
    ~Surface();
};
