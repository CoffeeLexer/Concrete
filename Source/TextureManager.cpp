#include "TextureManager.h"
#include <stdexcept>

#include "Scope.h"
#include "Texture.h"



//VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,

void TextureManager::createBuffer(VkDeviceSize size, const BufferCreateInfo bufferCI)
{
    const auto device = scope.getDevice().getVkDevice();
    const auto physicalDevice = scope.getDevice().getVkPhysicalDevice();

    Buffer buffer{};

    VkBufferCreateInfo ci {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = size,
        .usage = bufferCI.usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    if(vkCreateBuffer(device, &ci, nullptr, &buffer.buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed buffer creation");
    }

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &requirements);

    uint32_t memoryIndex = 0;

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    uint32_t filter = requirements.memoryTypeBits;
    VkMemoryPropertyFlags properties;
    for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)) {
            memoryIndex = i;
            break;
        }
    }

    VkMemoryAllocateInfo ai {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = requirements.size,
        .memoryTypeIndex = memoryIndex,
    };

    if(vkAllocateMemory(device, &ai, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("Failed allocate memory");
    }

    vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

    void *data;
    vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, (size_t)imageSize);
    vkUnmapMemory(device, stagingBufferMemory);
}

Texture TextureManager::createTexture(const char *filename)
{
    Texture texture;

    Buffer staging{};
    VkDeviceSize imageSize = width * height * channels;




}

