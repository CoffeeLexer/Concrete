#pragma once

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"

#include "vulkan/vulkan.h"

class Instance : public Handle<VkInstance>
{
    friend Scope::Scope();
    ScopeLink scope;
    explicit Instance(Scope *scope);
public:
    void Create();
    void Destroy();
};
