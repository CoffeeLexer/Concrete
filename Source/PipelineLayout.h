#pragma once

#include <vulkan/vulkan.h>
#include "Handle.h"
#include "Link.h"

class Engine;

class PipelineLayout
    : public Handle<VkPipelineLayout>
    , public Link<Engine>
{
public:
    explicit PipelineLayout(Engine *engine);
    ~PipelineLayout(); 
};