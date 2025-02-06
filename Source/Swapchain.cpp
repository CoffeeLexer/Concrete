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
    , currentImage(0)
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
    AllocateCommandPool();
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

void Swapchain::AllocateCommandPool()
{
    VkCommandPoolCreateInfo pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };

    VkDevice &device = engine;
    vkCreateCommandPool(device, &pool_ci, nullptr, &commandPool);

    VkCommandBufferAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = imageCount,
    };

    commandBuffers.resize(imageCount);
    vkAllocateCommandBuffers(device, &ai, commandBuffers.data());
}

void Swapchain::Draw()
{
    Device &device = engine;
    // vkDeviceWaitIdle(device);
    VkCommandBuffer &commandBuffer = commandBuffers[currentImage];
    // vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };

    uint32_t imageIndex;
    VkResult status = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
        imageSemaphores[currentImage], nullptr, &imageIndex);

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkClearValue clearValue = {
        .color = VkClearColorValue {
            .float32 = {1.0f, 0.0f, 0.0f, 1.0f},
        },
    };

    VkRect2D renderArea = {
        .offset = VkOffset2D {0, 0},
        .extent = extent,
    };

    VkRenderPassBeginInfo renderPassBegin = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = *renderPass,
        .framebuffer = framebuffers[currentImage],
        .renderArea = renderArea,
        .clearValueCount = 1,
        .pClearValues = &clearValue,
    };

    Pipeline &pipeline = engine;
    vkCmdBeginRenderPass(commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {
        .offset = {0, 0},
        .extent = extent,
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);

    status = vkEndCommandBuffer(commandBuffer);
    if (status != VK_SUCCESS)
    {
        throw std::runtime_error("Failed draw cmd");
    }

    VkSemaphore imageSemaphore[] = {
        imageSemaphores[currentImage],
    };

    VkSemaphore renderSemaphore[] = {
        renderSemaphores[currentImage],
    };

    VkPipelineStageFlags waitStage[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = imageSemaphore,
        .pWaitDstStageMask = waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = renderSemaphore,
    };

    status = vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, nullptr);
    if (status != VK_SUCCESS)
    {
        throw std::runtime_error("Failed queue submit");
    }

    VkSwapchainKHR swapchains[] = {
        swapchain,
    };

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = renderSemaphore,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    status = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
}

VkRenderPass Swapchain::GetRenderPass()
{
    return *renderPass;
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

Swapchain::operator VkSwapchainKHR&()
{
    return swapchain;
}
