#pragma once

#include "Handle.h"
#include "vulkan/vulkan.h"

class Engine;

class Surface : public Handle<VkSurfaceKHR>
{
protected:
    Surface() = default;
public:
    ~Surface();
    VkSurfaceCapabilitiesKHR GetCaps();
    VkSurfaceFormatKHR GetBestFormat();
};
