#pragma once
#include "vulkan/vulkan.h"

class Engine;

class Instance
{
    Engine &engine;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
public:
    operator VkInstance&();

    Instance(Engine &engine);
    ~Instance();
};
