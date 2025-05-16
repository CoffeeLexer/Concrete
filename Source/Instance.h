#pragma once

#include "vulkan/vulkan.h"

class Scope;
class Instance
{
    Scope &scope;
    VkInstance instance;
public:
    explicit Instance(Scope &scope);
    ~Instance();
    Instance(const Instance&) = delete;
    Instance& operator=(const Instance&) = delete;

    const VkInstance& getVkInstance() { return instance; }
};
