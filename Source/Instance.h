#pragma once

#include "Handle.h"
#include "vulkan/vulkan.h"

class Scope;
class Instance
{
    Scope &scope;
    VkInstance instance;
public:
    explicit Instance(Scope &scope);
    ~Instance();

    Handle<VkInstance> getVkInstance{instance};
};
