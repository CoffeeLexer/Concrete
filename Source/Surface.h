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
    explicit Surface(Scope *scope);
    void Create();
    void Destroy();
public:
    VkSurfaceCapabilitiesKHR GetCaps();
    VkSurfaceFormatKHR GetBestFormat();
    std::vector<VkSurfaceFormatKHR> GetFormats();
};
