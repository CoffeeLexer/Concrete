#pragma once

#include <vector>

#include "Link.h"
#include "Handle.h"
#include "vulkan/vulkan.h"

class Engine;

class Surface
    : public Handle<VkSurfaceKHR>
    , public Link<Engine>
{
public:
    explicit Surface(Engine *engine);
    ~Surface();
    VkSurfaceCapabilitiesKHR GetCaps();
    VkSurfaceFormatKHR GetBestFormat();
    std::vector<VkSurfaceFormatKHR> GetFormats();
};
