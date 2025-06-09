#pragma once

#include "Traits.h"
#include "vulkan/vulkan.h"

class Scope;
class Texture;

struct Buffer {
    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
};

struct BufferCreateInfo {
    VkBufferUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

struct Image {
    VkImage image;
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

    Buffer createBuffer(VkDeviceSize size, const BufferCreateInfo ci);
    Image createImage(ImageCreateInfo ci);

public:
    TextureManager(Scope &scope) : scope(scope) {}
    Texture createTexture(const char* filename);
};
