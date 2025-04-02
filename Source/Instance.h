#pragma once
#include "Handle.h"
#include "Scope.h"
#include "vulkan/vulkan.h"

class Engine;

class Instance : public Scope, public Handle<VkInstance>
{
    friend class Scope;
    Instance() = default;
public:
    void Create();
    void Destroy();
};
