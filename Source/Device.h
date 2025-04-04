#pragma once

#include "vulkan/vulkan.h"
#include <vector>

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"

class Device : public Handle<VkDevice>
{
    friend Scope::Scope(), Scope::~Scope();
    ScopeLink scope;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uint32_t graphicsIndex;
    uint32_t presentIndex;

    void Create();
    void Destroy();

    explicit Device(Scope *scope);

    std::vector<VkBool32> GetPresentSupportVector();

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    std::tuple<uint32_t, uint32_t> PickQueueFamily();
public:
    [[nodiscard]] uint32_t GetGraphicsIndex() const;
    [[nodiscard]] uint32_t GetPresentIndex() const;

    [[nodiscard]] VkQueue GetGraphicsQueue() const;
    [[nodiscard]] VkQueue GetPresentQueue() const;
};