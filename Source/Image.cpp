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
        .queueFamilyIndexCount = ,
        .pQueueFamilyIndices = ,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

}