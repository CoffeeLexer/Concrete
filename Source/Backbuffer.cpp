#include "Backbuffer.h"

#include <stdexcept>
#include <algorithm>
#include <thread>
#include <chrono>

#include "Engine.h"

VkPresentModeKHR Backbuffer::GetBestPresentMode()
{
    uint32_t count;
    const VkSurfaceKHR surface = Owner().surface;
    const VkPhysicalDevice physicalDevice = Owner().physicalDevice;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    std::vector<VkPresentModeKHR> modes = {};
    modes.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());

    const VkPresentModeKHR priorities[] = {
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

VkFormat Backbuffer::GetFormat()
{
    return this->format;
}

Backbuffer::Backbuffer(Engine *engine)
    : Link(engine)
    , currentImage(0)
{
    VkPresentModeKHR presentMode = GetBestPresentMode();
    Surface &surface = Owner().surface;
    const auto caps = surface.GetCaps();
    VkSurfaceFormatKHR format = surface.GetBestFormat();
    this->format = format.format;
    this->extent = caps.currentExtent;

    imageCount = 3;
    imageCount = std::max(imageCount, caps.minImageCount);
    if (caps.maxImageCount) // 0 => no limit
    {
        imageCount = std::min(imageCount, caps.maxImageCount);
    }

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
    
    Device &deviceWrapper = Owner().device;
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
    VkResult result = vkCreateSwapchainKHR(device, &ci, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed creating swapchain");
    }

    vkGetSwapchainImagesKHR(device, handle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, handle, &imageCount, images.data());

    renderPass = new RenderPass(owner);

    CreateImageViews();
    CreateFramebuffers();
    CreateSemaphores();
    AllocateCommandPool();
    CreateFences();
}

Backbuffer::~Backbuffer()
{
    VkDevice device = Owner().device;
    delete renderPass;
    vkDestroySwapchainKHR(device, handle, nullptr);
}

void Backbuffer::CreateImageViews()
{
    imageViews.resize(imageCount);

    VkDevice device = Owner().device;

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

void Backbuffer::CreateFences()
{
    renderFences.resize(imageCount);

    VkDevice device = Owner().device;
    VkFenceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (uint32_t i = 0; i < imageCount; i++)
    {
        VkResult status = vkCreateFence(device, &ci, nullptr, &renderFences[i]);
        if (status != VK_SUCCESS)
        {
            throw std::runtime_error("Failed fence create");
        }
    }
}

void Backbuffer::CreateFramebuffers()
{
    framebuffers.resize(imageCount);

    VkDevice device = Owner().device;
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

void Backbuffer::AllocateCommandPool()
{
    VkCommandPoolCreateInfo pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };

    VkDevice device = Owner().device;
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

void Backbuffer::Draw()
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Device &device = Owner().device;
    VkFramebuffer &framebuffer = framebuffers[currentImage];
    VkFence &fence = renderFences[currentImage];
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
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
    VkResult status = vkAcquireNextImageKHR(device, handle, UINT64_MAX,
        imageSemaphores[currentImage], nullptr, &imageIndex);

    vkResetFences(device, 1, &fence);

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkClearValue clearValue = {
        .color = VkClearColorValue {
            .float32 = {1.0f, 0.0f, 1.0f, 1.0f},
        },
    };

    // ui.Start();
    // ImGui::ShowDemoWindow();
    // ui.End(commandBuffer, extent);

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

    Pipeline pipeline = Owner().pipeline;
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
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
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

    status = vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, fence);
    if (status != VK_SUCCESS)
    {
        throw std::runtime_error("Failed queue submit");
    }

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = renderSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &handle,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    status = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
    if (status != VK_SUCCESS)
    {
        throw std::runtime_error("Failed queue present");
    }

    currentImage = ++currentImage % imageCount;
}

uint32_t Backbuffer::GetIndex()
{
    return currentImage;
}

VkRenderPass Backbuffer::GetRenderPass()
{
    return *renderPass;
}

uint32_t Backbuffer::GetImageCount()
{
    return imageCount;
}

void Backbuffer::CreateSemaphores()
{
    renderSemaphores.resize(imageCount);
    imageSemaphores.resize(imageCount);

    VkSemaphoreCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkResult status = VK_SUCCESS;
    VkDevice device = Owner().device;
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

VkImageView Backbuffer::GetView(uint32_t i)
{
    return imageViews[i];
}

VkExtent2D Backbuffer::GetExtent()
{
    return extent;
}
