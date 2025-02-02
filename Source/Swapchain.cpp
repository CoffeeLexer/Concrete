#include "Swapchain.h"

#include <stdexcept>
#include <algorithm>

#include "Engine.h"

VkPresentModeKHR Swapchain::GetBestPresentMode()
{
    uint32_t count;
    const VkSurfaceKHR &surface = engine;
    const VkPhysicalDevice &physicalDevice = engine;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    std::vector<VkPresentModeKHR> modes = {};
    modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());

    VkPresentModeKHR priorities[] = {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_KHR,
        VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
    };

    for (const auto& p : priorities)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (p == modes.at(i))
                return p;
        }
    }
    return modes.at(0);
}

VkSurfaceFormatKHR Swapchain::GetBestSurfaceFormat()
{
    VkPhysicalDevice physicalDevice = engine;
    VkSurfaceKHR surface = engine;
    uint32_t count;

    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats;
    formats.resize(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, formats.data());

    VkFormat priorities[] = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SRGB,
    };

    for (const auto& p : priorities)
    {
        for (uint32_t i = 0; i < count; i++)
        {
            if (p == formats.at(i).format)
                return formats.at(i);
        }
    }
    return formats.at(0);
}


VkSurfaceCapabilitiesKHR Swapchain::GetSurfaceCaps()
{
    VkPhysicalDevice physicalDevice = engine;
    VkSurfaceKHR surface = engine;
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps);
    return caps; 
}

VkFormat Swapchain::GetFormat()
{
    return this->format;
}

VkExtent2D Swapchain::GetExtent()
{
    return this->extent;
}

Swapchain::Swapchain(Engine &engine)
    : engine(engine)
{
    VkPresentModeKHR presentMode = GetBestPresentMode();
    const auto caps = GetSurfaceCaps();
    VkSurfaceFormatKHR format = GetBestSurfaceFormat();
    this->format = format.format;
    this->extent = caps.currentExtent;

    imageCount = 3;
    imageCount = std::max(imageCount, caps.minImageCount);
    if (caps.maxImageCount) // 0 => no limit
    {
        imageCount = std::min(imageCount, caps.maxImageCount);
    }

    VkSurfaceKHR surface = engine;
    VkSwapchainCreateInfoKHR ci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = caps.minImageCount,
        .imageFormat = this->format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = this->extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = nullptr,
    };

    ci.compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(
        caps.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
    
    Device &deviceWrapper = engine;
    uint32_t graphicsIndex = deviceWrapper.GetGraphicsIndex();
    uint32_t presentIndex = deviceWrapper.GetPresentIndex();
    uint32_t queueIndices[] = {graphicsIndex, presentIndex};

    if (graphicsIndex == presentIndex)
    {
        ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        ci.queueFamilyIndexCount = 0;
        ci.pQueueFamilyIndices = nullptr;
    }
    else
    {
        ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        ci.queueFamilyIndexCount = 2;
        ci.pQueueFamilyIndices = queueIndices;
    }

    VkDevice device = deviceWrapper;
    VkResult result = vkCreateSwapchainKHR(device, &ci, nullptr, &swapchain);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed creating swapchain");
    }

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

    renderPass = new RenderPass(engine);

    CreateImageViews();
    CreateFramebuffers();
    CreateSemaphores();
}

Swapchain::~Swapchain()
{
    VkDevice device = engine;
    delete renderPass;
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void Swapchain::CreateImageViews()
{
    imageViews.resize(imageCount);

    VkDevice &device = engine;

    for (uint32_t i = 0; i < imageViews.size(); i++)
    {
        VkImageViewCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components = VkComponentMapping {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = VkImageSubresourceRange {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        VkResult status = vkCreateImageView(device, &ci, nullptr, &imageViews[i]);
        if (status != VK_SUCCESS)
        {
            throw std::runtime_error("Failed swapchain image view");
        }
    }
}

void Swapchain::CreateFramebuffers()
{
    framebuffers.resize(imageCount);

    VkDevice &device = engine;
    for (uint32_t i = 0; i < imageCount; i++)
    {
        VkImageView attachments[] = {
            imageViews[i],
        };

        VkFramebufferCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = *renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        VkResult status = vkCreateFramebuffer(device, &ci, nullptr, &framebuffers[i]);
        if (status != VK_SUCCESS)
        {
            throw std::runtime_error("Failed framebuffers create");
        }
    }

}

VkRenderPass Swapchain::GetRenderPass()
{
    return *renderPass;
}

uint32_t Swapchain::GetImageCount()
{
    return imageCount;
}

VkFramebuffer Swapchain::GetFramebuffer(uint32_t i)
{
    return framebuffers[i];
}

void Swapchain::CreateSemaphores()
{
    renderSemaphores.resize(imageCount);
    imageSemaphores.resize(imageCount);

    VkSemaphoreCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkResult status = VK_SUCCESS;
    VkDevice device = engine;
    for (uint32_t i = 0; i < imageCount; i++)
    {
        status = vkCreateSemaphore(device, &ci, nullptr, &renderSemaphores[i]);
        if (status != VK_SUCCESS)
        {
            throw std::runtime_error("Failed semaphore creation (wait)");
        }
        status = vkCreateSemaphore(device, &ci, nullptr, &imageSemaphores[i]);
        if (status != VK_SUCCESS)
        {
            throw std::runtime_error("Failed semaphore creation (availible)");
        }
    }
}

VkSemaphore Swapchain::GetRenderSemaphore(uint32_t i)
{
    return renderSemaphores[i];
}

VkSemaphore Swapchain::GetImageSemaphore(uint32_t i)
{
    return imageSemaphores[i];
}

Swapchain::operator VkSwapchainKHR&()
{
    return swapchain;
}
