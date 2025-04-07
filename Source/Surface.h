#pragma once

#include <vector>

#include "ScopeLink.h"
#include "Scope.h"
#include "Handle.h"
#include "vulkan/vulkan.h"


class Surface : public Handle<VkSurfaceKHR>
{
    friend Scope::Scope(), Scope::~Scope();
    ScopeLink scope;
    VkSurfaceFormatKHR format;
    explicit Surface(Scope *scope);

    VkSurfaceFormatKHR selectBestFormat();
    std::vector<VkSurfaceFormatKHR> GetFormats();

    void Create();
    void Destroy();
public:
    VkSurfaceCapabilitiesKHR GetCaps();
};
