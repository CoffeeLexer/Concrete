#pragma once

#include <vulkan/vulkan.h>
#include "Handle.h"
#include "Link.h"

class Engine;

class Image
    : public Handle<VkImage>
    , public Handle<VkDeviceMemory>
    , public Handle<VkImageView>
    , public Link<Engine>
{
    void CreateView();
    void CreateMemory();
public:
    explicit Image(Engine *engine, VkExtent3D extent);
};
