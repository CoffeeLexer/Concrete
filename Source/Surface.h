#pragma once

#include "vulkan/vulkan.h"

class Engine;

class Surface
{
    Engine &engine;
    VkSurfaceKHR surface;
public:
    operator VkSurfaceKHR&();
    Surface(Engine &engine);
    ~Surface();
};
