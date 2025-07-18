#pragma once

#include "Traits.h"
#include "vulkan/vulkan.h"

class Scope;
class Texture;

struct Buffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkMemoryRequirements requirements = {};
};

struct BufferCreateInfo {
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

struct Image {
    VkImage image;
    VkDeviceMemory memory;
};

struct ImageCreateInfo {
    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

class TextureManager : DisableCopy {
    Scope& scope;

    VkPhysicalDeviceMemoryProperties memoryProperties = {};
    const uint32_t cmdPoolCapacity = 4;
    VkCommandPool cmdPool = VK_NULL_HANDLE;

    transition();

    [[nodiscard]] VkDeviceMemory allocateMemory(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties);
    Buffer createBuffer(VkDeviceSize size, const BufferCreateInfo ci);
    Image createImage(ImageCreateInfo ci);

public:
    TextureManager(Scope &scope);
    Texture createTexture(const char* filename);
};
