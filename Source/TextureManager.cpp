#include "TextureManager.h"
#include <stdexcept>
#include "stb_image.h"

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

    buffer.memory = allocateMemory(requirements, bufferCI.properties);

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
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);


    Texture texture;
    VkDeviceSize imageSize = texWidth * texHeight * texChannels;
    Buffer staging = createBuffer(imageSize, CPU_TO_GPU);

    const VkDevice device = scope.getDevice().getVkDevice();

    // Transfer CPU to GPU
    void *data;
    vkMapMemory(device, staging.memory, 0, imageSize, 0, &data);
    memcpy(data, pixels, (size_t)imageSize);
    vkUnmapMemory(device, staging.memory);

    ImageCreateInfo imageCI {
        .width = static_cast<uint32_t>(texWidth),
        .height = static_cast<uint32_t>(texHeight),
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    };

    Image image = createImage(imageCI);

    transition(image.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    bufferToImage(staging.buffer, image.image, texWidth, texHeight);
    transition(image.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device, staging.buffer, nullptr);
    vkFreeMemory(device, staging.memory, nullptr);
    return texture;
}

void TextureManager::bufferToImage(VkBuffer buf, VkImage img, uint32_t width, uint32_t height)
{
    VkCommandBufferAllocateInfo ai {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = cmdPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    auto device = scope.getDevice().getVkDevice();

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device, &ai, &cmd);

    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    vkBeginCommandBuffer(cmd, &beginInfo);

    VkBufferImageCopy region = {
         .bufferOffset = 0,
         .bufferRowLength = 0,
         .bufferImageHeight = 0,
         .imageSubresource = VkImageSubresourceLayers {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
             },
         .imageOffset = {0, 0, 0},
         .imageExtent = {width, height, 1},
    };

    vkCmdCopyBufferToImage(cmd, buf, img, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    vkEndCommandBuffer(cmd);
    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr,
    };

    auto gfxQueue = scope.getDevice().getQueues().graphics.queue;
    vkQueueSubmit(gfxQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gfxQueue);

    vkFreeCommandBuffers(device, cmdPool, 1, &cmd);
}

void TextureManager::allocateCmdPool()
{
    auto queueFamily = scope.getDevice().getQueues().transfer.index;

    VkCommandPoolCreateInfo pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamily,
    };

    VkDevice device = scope.getDevice().getVkDevice();
    vkCreateCommandPool(device, &pool_ci, nullptr, &cmdPool);
}

void TextureManager::transition(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBufferAllocateInfo ai {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = cmdPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    auto device = scope.getDevice().getVkDevice();

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device, &ai, &cmd);

    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    vkBeginCommandBuffer(cmd, &beginInfo);

    VkImageMemoryBarrier barrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,
        .dstAccessMask = 0,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::runtime_error("Unsupported transition");
    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    vkEndCommandBuffer(cmd);

    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .pWaitDstStageMask = nullptr,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = nullptr,
    };

    auto gfxQueue = scope.getDevice().getQueues().graphics.queue;
    vkQueueSubmit(gfxQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(gfxQueue);

    vkFreeCommandBuffers(device, cmdPool, 1, &cmd);
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

    image.memory = allocateMemory(requirements, imageCI.properties);

    if (vkBindImageMemory(device, image.image, image.memory, 0) != VK_SUCCESS)
        throw std::runtime_error("Failed bind image memory");

    return image;
}

TextureManager::TextureManager(Scope &scope) : scope(scope) {
    const VkPhysicalDevice &physicalDevice = scope.getDevice().getVkPhysicalDevice();
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
}
