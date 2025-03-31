#pragma once
#include "Handle.h"
#include "Ownership.h"
#include "vulkan/vulkan.h"

class Engine;

class Instance
    : public Handle<VkInstance>
    , public virtual Owned
{
public:
    Instance() = default;
    ~Instance() override = default;
    void Create();
    void Destroy();
};
