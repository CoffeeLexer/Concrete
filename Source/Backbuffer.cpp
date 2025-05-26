#include "Backbuffer.h"

#include <stdexcept>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cassert>

#include "Scope.h"
#include "GLFW/glfw3.h"
#include "Shader.h"

Backbuffer::Backbuffer(Scope &scope) : scope(scope)
{
    imageCount = 3;

    createSwapchain();

    frames = std::vector<Frame>(imageCount);

    createImages();
    createImageViews();
    createRenderPass();
    createFramebuffers();

    createSemaphores();
    createFences();

    allocateCommandPool();
}
Backbuffer::~Backbuffer()
{
    deallocateCommandPool();
    destroyFences();
    destroySemaphores();
    destroyFramebuffers();
    destroyRenderPass();
    destroyImageViews();
    destroyImages();
    destroySwapchain();
}

void Backbuffer::destroyRenderPass()
{
    const auto device = scope.getDevice().getVkDevice();
    vkDestroyRenderPass(device, renderPass, nullptr);
}

void Backbuffer::createRenderPass()
{
    VkAttachmentDescription attachment = {
        .flags = 0,
        .format = surfaceFormat.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, // backbuffer
    };

    VkAttachmentReference color_attachment_ref = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_ref,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    VkRenderPassCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &attachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 0,
        .pDependencies = nullptr,
    };

    VkDevice device = scope.getDevice().getVkDevice();
    VkResult status = vkCreateRenderPass(device, &ci, nullptr, &renderPass);
    if (status != VK_SUCCESS)
        throw std::runtime_error("RenderPass::Create Failed");
}
void Backbuffer::destroyImages()
{
    // vkDestroySwapchain handles this
}

void Backbuffer::createImages()
{
    const auto device = scope.getDevice().getVkDevice();
    std::vector<VkImage> images{frames.size()};
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

    auto it = frames.begin();
    auto it_img = images.begin();
    while (it != frames.end() && it_img != images.end())
    {
        it->image = *it_img;
        ++it;
        ++it_img;
    }
}

struct SurfaceCaps : VkSurfaceCapabilitiesKHR {
    SurfaceCaps(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, this);
    }
};

struct SurfaceFormats : std::vector<VkSurfaceFormatKHR> {
    SurfaceFormats(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
        uint32_t count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
        this->resize(count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, this->data());
    }
};

void Backbuffer::selectSurfaceFormat()
{
    const VkFormat priorities[] = {
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_FORMAT_R8G8B8A8_SRGB,
    };

    const auto formats = SurfaceFormats{scope.getDevice().getVkPhysicalDevice(), scope.getWindow().getVkSurface()};
    for (const auto& p : priorities)
    {
        for (const auto& format : formats)
        {
            if (p == format.format)
            {
                surfaceFormat = format;
                return;
            }
        }
    }
    surfaceFormat = formats.at(0);
}

std::vector<VkPresentModeKHR> surfacePresentModes(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
    std::vector<VkPresentModeKHR> formats;
    uint32_t count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
    formats.resize(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, formats.data());
    return formats;
}

void Backbuffer::selectPresentMode()
{
    const auto presentModes = surfacePresentModes(scope.getDevice().getVkPhysicalDevice(), scope.getWindow().getVkSurface());

    const VkPresentModeKHR priorities[] = {
        VK_PRESENT_MODE_MAILBOX_KHR,
        VK_PRESENT_MODE_FIFO_KHR,
        VK_PRESENT_MODE_FIFO_RELAXED_KHR,
        VK_PRESENT_MODE_IMMEDIATE_KHR,
    };

    for (const auto& p : priorities)
    {
        for (uint32_t i = 0; i < presentModes.size(); i++)
        {
            if (p == presentModes.at(i))
            {
                presentMode = p;
                return;
            }
        }
    }
    presentMode = presentModes.at(0);
}

void Backbuffer::destroySwapchain()
{
    const auto device = scope.getDevice().getVkDevice();
    vkDestroySwapchainKHR(device, swapchain, nullptr);
}

void Backbuffer::createSwapchain()
{
    selectPresentMode();
    const auto caps = SurfaceCaps{scope.getDevice().getVkPhysicalDevice(), scope.getWindow().getVkSurface()};
    selectSurfaceFormat();

    const auto windowInfo = scope.getWindow().getInfo();
    extent = {};
    extent.width = windowInfo.width;
    extent.height = windowInfo.height;

    assert(imageCount);
    imageCount = std::max(imageCount, caps.minImageCount);
    if (caps.maxImageCount) // 0 => no limit
        imageCount = std::min(imageCount, caps.maxImageCount);

    VkSwapchainCreateInfoKHR ci = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = scope.getWindow().getVkSurface(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = nullptr,
    };

    const auto &queues = scope.getDevice().getQueues();
    uint32_t graphicsIndex = queues.graphics.index;
    uint32_t presentIndex = queues.present.index;
    uint32_t queueIndices[] = {graphicsIndex, presentIndex};

    if (queues.sameFamily())
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

    const auto device = scope.getDevice().getVkDevice();
    VkResult result = vkCreateSwapchainKHR(device, &ci, nullptr, &swapchain);
    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed creating swapchain");

    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
}

void Backbuffer::destroyImageViews()
{
    const auto device = scope.getDevice().getVkDevice();
    for(auto& frame : frames)
        vkDestroyImageView(device, frame.imageView, nullptr);
}

void Backbuffer::createImageViews()
{
    VkDevice device = scope.getDevice().getVkDevice();

    for (auto& frame : frames)
    {
        VkImageViewCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = frame.image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = surfaceFormat.format,
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

        VkResult status = vkCreateImageView(device, &ci, nullptr, &frame.imageView);
        if (status != VK_SUCCESS)
            throw std::runtime_error("Failed swapchain image view");
    }
}

void Backbuffer::destroyFences()
{
    const auto device = scope.getDevice().getVkDevice();

    for(auto& frame : frames)
        vkDestroyFence(device, frame.renderFence, nullptr);
}

void Backbuffer::createFences()
{
    VkDevice device = scope.getDevice().getVkDevice();
    VkFenceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };

    for (auto &frame : frames)
    {
        VkResult status = vkCreateFence(device, &ci, nullptr, &frame.renderFence);
        if (status != VK_SUCCESS)
            throw std::runtime_error("Failed fence create");
    }
}

void Backbuffer::destroyFramebuffers()
{
    const auto device = scope.getDevice().getVkDevice();
    for(auto& frame : frames)
        vkDestroyFramebuffer(device, frame.framebuffer, nullptr);
}

void Backbuffer::createFramebuffers()
{
    const auto device = scope.getDevice().getVkDevice();
    for (auto &frame : frames)
    {
        VkImageView attachments[] = {
            frame.imageView
        };

        VkFramebufferCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        VkResult status = vkCreateFramebuffer(device, &ci, nullptr, &frame.framebuffer);
        if (status != VK_SUCCESS)
            throw std::runtime_error("Failed framebuffers create");
    }
}

void Backbuffer::deallocateCommandPool()
{
    const auto device = scope.getDevice().getVkDevice();
    vkDestroyCommandPool(device, commandPool, nullptr);
}

void Backbuffer::allocateCommandPool()
{
    VkCommandPoolCreateInfo pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };

    VkDevice device = scope.getDevice().getVkDevice();
    vkCreateCommandPool(device, &pool_ci, nullptr, &commandPool);

    VkCommandBufferAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = imageCount,
    };

    std::vector<VkCommandBuffer> buffers{frames.size()};
    vkAllocateCommandBuffers(device, &ai, buffers.data());

    auto it = frames.begin();
    auto it_buf = buffers.begin();
    while (it != frames.end() && it_buf != buffers.end())
    {
        it->commandBuffer = *it_buf;
        ++it;
        ++it_buf;
    }
}

void Backbuffer::beginFrame()
{
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));
    VkDevice device = scope.getDevice().getVkDevice();
    Frame &frame = frames[currentFrame];
    vkWaitForFences(device, 1, &frame.renderFence, VK_TRUE, UINT64_MAX);
    // vkDeviceWaitIdle(device);

    // vkResetCommandBuffer(commandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };

    vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
        frame.imageSemaphore, nullptr, &frame.imageIndex);

    vkResetFences(device, 1, &frame.renderFence);

    vkBeginCommandBuffer(frame.commandBuffer, &beginInfo);
    VkClearValue clearValue = {
        .color = VkClearColorValue {
            .float32 = {1.0f, 0.0f, 1.0f, 1.0f},
        },
    };
    VkRect2D renderArea = {
        .offset = VkOffset2D {0, 0},
        .extent = extent,
    };

    VkRenderPassBeginInfo renderPassBegin = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = frame.framebuffer,
        .renderArea = renderArea,
        .clearValueCount = 1,
        .pClearValues = &clearValue,
    };

//    Pipeline pipeline = Owner().pipeline;
    vkCmdBeginRenderPass(frame.commandBuffer, &renderPassBegin, VK_SUBPASS_CONTENTS_INLINE);
//    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vkCmdSetViewport(frame.commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {
        .offset = {0, 0},
        .extent = extent,
    };
    vkCmdSetScissor(frame.commandBuffer, 0, 1, &scissor);
}

void Backbuffer::draw(Shader &shader)
{
    Frame &frame = frames[currentFrame];
    vkCmdBindPipeline(frame.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shader.getVkPipeline());
    vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);
}

void Backbuffer::endFrame()
{
    Frame &frame = frames[currentFrame];
    vkCmdEndRenderPass(frame.commandBuffer);

    if (vkEndCommandBuffer(frame.commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed draw cmd");

    VkSemaphore imageSemaphore[] = {
        frame.imageSemaphore,
    };

    VkSemaphore renderSemaphore[] = {
        frame.renderSemaphore,
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
        .pCommandBuffers = &frame.commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = renderSemaphore,
    };

    if (vkQueueSubmit(scope.getDevice().getQueues().graphics.queue, 1, &submitInfo, frame.renderFence) != VK_SUCCESS)
        throw std::runtime_error("Failed queue submit");

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = renderSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &frame.imageIndex,
        .pResults = nullptr,
    };

    if (vkQueuePresentKHR(scope.getDevice().getQueues().present.queue, &presentInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed queue present");

    currentFrame = ++currentFrame % imageCount;
}

void Backbuffer::waitIdle() {
    const VkDevice device = scope.getDevice().getVkDevice();
    vkDeviceWaitIdle(device);
}

void Backbuffer::destroySemaphores()
{
    const auto device = scope.getDevice().getVkDevice();
    for(auto& frame : frames)
    {
        vkDestroySemaphore(device, frame.renderSemaphore, nullptr);
        vkDestroySemaphore(device, frame.imageSemaphore, nullptr);
    }
}

void Backbuffer::createSemaphores()
{
    VkSemaphoreCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkResult status;
    VkDevice device = scope.getDevice().getVkDevice();
    for (auto &frame : frames)
    {
        status = vkCreateSemaphore(device, &ci, nullptr, &frame.renderSemaphore);
        if (status != VK_SUCCESS)
            throw std::runtime_error("Failed semaphore creation (wait)");

        status = vkCreateSemaphore(device, &ci, nullptr, &frame.imageSemaphore);
        if (status != VK_SUCCESS)
            throw std::runtime_error("Failed semaphore creation (available)");
    }
}
