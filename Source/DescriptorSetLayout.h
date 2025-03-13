#pragma once

#include <vulkan/vulkan.h>
#include "Handle.h"
#include "Link.h"
class Engine;

class DescriptorSetLayout
    : public Handle<VkDescriptorSetLayout>
    , public Link<Engine>
{
public:
    DescriptorSetLayout(Engine *engine);
};
