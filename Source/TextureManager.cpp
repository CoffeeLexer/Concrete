#include "TextureManager.h"
#include <stdexcept>

#include "Scope.h"
#include "Texture.h"


Buffer TextureManager::createBuffer(VkDeviceSize size, const BufferCreateInfo bufferCI)
{
    const auto device = scope.getDevice().getVkDevice();
    const auto physicalDevice = scope.getDevice().getVkPhysicalDevice();

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

    Buffer buffer{};
    if(vkCreateBuffer(device, &ci, nullptr, &buffer.buffer) != VK_SUCCESS)
        throw std::runtime_error("Failed buffer creation");

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, buffer.buffer, &requirements);

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    uint32_t filter = requirements.memoryTypeBits;
    VkMemoryPropertyFlags properties;

    std::optional<uint32_t> memoryIndex = 0;
    for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memoryIndex = i;
            break;
        }
    }
    if (!memoryIndex.has_value())
        throw std::runtime_error("Failed to find requested memory");

    VkMemoryAllocateInfo ai {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = requirements.size,
        .memoryTypeIndex = memoryIndex.value(),
    };

    if(vkAllocateMemory(device, &ai, nullptr, &buffer.memory) != VK_SUCCESS)
        throw std::runtime_error("Failed allocate memory");

    if(vkBindBufferMemory(device, buffer.buffer, buffer.memory, 0) != VK_SUCCESS)
        throw std::runtime_error("Failed bind memory");

    return buffer;
}

const BufferCreateInfo CPU_TO_GPU = {
    .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    .properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
};

Texture TextureManager::createTexture(const char *filename)
{
    Texture texture;
    VkDeviceSize imageSize = width * height * channels;
    Buffer staging = createBuffer(imageSize, CPU_TO_GPU);

    const VkDevice device = scope.getDevice().getVkDevice();

    // Transfer CPU to GPU
    void *data;
    vkMapMemory(device, staging.memory, 0, imageSize, 0, &data);
    memcpy(data, pixels, (size_t)imageSize);
    vkUnmapMemory(device, staging.memory);

    createImage(texWidth, texHeight,
                VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                textureImage,
                textureImageMemory);
}

VkDeviceMemory TextureManager::allocateMemory(VkMemoryRequirements requirements, VkMemoryPropertyFlags properties)
{
    const VkPhysicalDevice &physicalDevice = scope.getDevice().getVkPhysicalDevice();
    const VkDevice &device = scope.getDevice().getVkDevice();

    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    uint32_t filter = requirements.memoryTypeBits;

    std::optional<uint32_t> memoryIndex = 0;
    for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memoryIndex = i;
            break;
        }
    }
    if (!memoryIndex.has_value())
        throw std::runtime_error("Failed to find requested memory");

    VkMemoryAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = requirements.size,
        .memoryTypeIndex = memoryIndex.value(),
    };

    VkDeviceMemory memory;
    if(vkAllocateMemory(device, &ai, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("Failed allocating device memory");

    return memory;
}

Image TextureManager::createImage(ImageCreateInfo imageCI)
{
    VkImageCreateInfo ci {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = imageCI.format,
        .extent = VkExtent3D {
            .width = imageCI.width,
            .height = imageCI.height,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = imageCI.tiling,
        .usage = imageCI.usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VkDevice device = scope.getDevice().getVkDevice();
    Image image{};

    if(vkCreateImage(device, &ci, nullptr, &image.image) != VK_SUCCESS)
        throw std::runtime_error("Failed to create image");

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(device, image.image, &requirements);

    VkMemoryAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = requirements.size,
        .memoryTypeIndex = ,
    };
}

