#include "Image.h"

#include "Engine.h"

Image::Image(Engine &engine, VkExtent3D extent)
    : engine(engine)
{
    VkImageCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D ,
        .format = VK_FORMAT_R8G8B8A8_UNORM,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 0,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VkDevice &device = engine;
    vkCreateImage(device, &ci, nullptr, &image);
    CreateMemory();
    CreateView();
}

void Image::CreateMemory()
{
    VkDevice &device = engine;
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(device, image, &requirements);

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = requirements.size,
        .memoryTypeIndex = 0,
    };

    vkAllocateMemory(device, &allocInfo, nullptr, &memory);
    vkBindImageMemory(device, image, memory, 0);
}

void Image::CreateView()
{
    VkComponentMapping colorMapping = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A,
    };
    VkImageSubresourceRange subresource = {
        .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };
    VkImageViewCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_D16_UNORM,
        .components = colorMapping,
        .subresourceRange = subresource,
    };

    VkDevice &device = engine;
    vkCreateImageView(device, &ci, nullptr, &view);
}
