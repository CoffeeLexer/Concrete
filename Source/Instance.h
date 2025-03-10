#pragma once
#include "Handle.h"
#include "vulkan/vulkan.h"

class Engine;

class Instance : public Handle<VkInstance>
{
protected:
    Instance() = default;
    void Create();
    ~Instance();
};
