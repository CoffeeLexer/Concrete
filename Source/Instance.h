#pragma once
#include "Handle.h"
#include "Link.h"
#include "vulkan/vulkan.h"

class Engine;

class Instance
    : public Handle<VkInstance>
    , public Link<Engine>
{
public:
    Instance(Engine *engine);
    ~Instance();
};
