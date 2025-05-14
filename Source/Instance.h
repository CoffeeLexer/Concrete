#pragma once

#include "Handle.h"
#include "Scope.h"

#include "vulkan/vulkan.h"

class Instance : public Handle<VkInstance>
{
    Scope &scope;
public:
    explicit Instance(Scope &scope);
    ~Instance();
};
