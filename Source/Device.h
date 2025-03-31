#pragma once

#include "vulkan/vulkan.h"
#include <vector>

#include "Handle.h"
#include "Link.h"
#include "Ownership.h"

class Engine;

class Device
    : public Handle<VkDevice>
    , public virtual Owned
{
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uint32_t graphicsIndex;
    uint32_t presentIndex;

    std::vector<VkBool32> GetPresentSupportVector();

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    std::tuple<uint32_t, uint32_t> PickQueueFamily();
public:
    explicit Device() = default;
    ~Device() = default;

    void Create();
    void Destroy();

    [[nodiscard]] uint32_t GetGraphicsIndex() const;
    [[nodiscard]] uint32_t GetPresentIndex() const;

    [[nodiscard]] VkQueue GetGraphicsQueue() const;
    [[nodiscard]] VkQueue GetPresentQueue() const;
};