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

class TextureManager : DisableCopy {
    Scope& scope;

    void createBuffer(VkDeviceSize size, const BufferCreateInfo ci)

public:
    TextureManager(Scope &scope) : scope(scope) {}
    Texture createTexture(const char* filename);
};
