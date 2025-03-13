#pragma once

#include <vulkan/vulkan.h>
#include "Handle.h"
#include "Link.h"

class Engine;

class PipelineLayout
    : public Handle<VkDevice>
    , public Link<Engine>
{
    VkPipelineLayout pipelineLayout;
    Engine &engine;
public:
    explicit PipelineLayout(Engine *engine);
    ~PipelineLayout(); 
};