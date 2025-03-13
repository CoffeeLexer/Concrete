#pragma once

#include "vulkan/vulkan.h"
#include "Handle.h"
#include "Link.h"

class Engine;

class RenderPass
    : public Handle<VkRenderPass>
    , public Link<Engine>
{
public:
    explicit RenderPass(Engine *engine);
    ~RenderPass();
};
