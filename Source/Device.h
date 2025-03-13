#pragma once

#include "vulkan/vulkan.h"
#include <vector>

#include "Handle.h"
#include "Link.h"

class Engine;

class Device
    : public Handle<VkDevice>
    , public Link<Engine>
{
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    uint32_t graphicsIndex;
    uint32_t presentIndex;

    std::vector<VkBool32> GetPresentSupportVector();

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties();
    std::tuple<uint32_t, uint32_t> PickQueueFamily();
public:
    explicit Device(Engine *engine);
    ~Device();

    [[nodiscard]] uint32_t GetGraphicsIndex() const;
    [[nodiscard]] uint32_t GetPresentIndex() const;

    [[nodiscard]] VkQueue GetGraphicsQueue() const;
    [[nodiscard]] VkQueue GetPresentQueue() const;
};