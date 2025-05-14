#pragma once

#include <vector>

#include "Scope.h"
#include "Handle.h"
#include "vulkan/vulkan.h"

class Surface : public Handle<VkSurfaceKHR>
{
    Scope scope;
    VkSurfaceFormatKHR format;

    VkSurfaceFormatKHR selectBestFormat();
    std::vector<VkSurfaceFormatKHR> GetFormats();
public:
    explicit Surface(Scope &scope);
    ~Surface();
    VkSurfaceCapabilitiesKHR GetCaps();
};
