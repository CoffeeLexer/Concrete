#pragma once
#include "vulkan/vulkan.h"

class Engine;

class Instance
{
    const Engine &engine;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
public:
    operator const VkInstance&() const
    {
        return instance;
    }

    Instance(const Engine &engine);
    ~Instance();
};