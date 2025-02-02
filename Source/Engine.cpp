#include "Engine.h"

#include <stdexcept>

Engine::Engine()
    : instance(Instance(*this))
    , physicalDevice(PhysicalDevice(*this))
    , window(Window(*this))
    , surface(Surface(*this))
    , device(Device(*this))
    , swapchain(Swapchain(*this))
    , pipeline(Pipeline(*this))
{

}

Engine::~Engine()
{
}

void Engine::run()
{
    VkCommandPoolCreateInfo pool_ci = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = 0,
    };

    VkCommandPool commandPool;
    vkCreateCommandPool(device, &pool_ci, nullptr, &commandPool);

    VkCommandBufferAllocateInfo ai = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 3,
    };

    VkCommandBuffer commandBuffers[3];
    vkAllocateCommandBuffers(device, &ai, commandBuffers);

    uint32_t currentImage = 0;
    while (window.IsValid())
    {
        VkCommandBuffer &commandBuffer = commandBuffers[currentImage];

        VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr,
        };

        uint32_t imageIndex;
        VkResult status = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
            swapchain.GetImageSemaphore(currentImage), nullptr, &imageIndex);

        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        VkClearValue clearValue = {
            .color = VkClearColorValue {
                .float32 = {1.0f, 0.0f, 0.0f, 1.0f},
            },
        };

        VkExtent2D extent = swapchain.GetExtent();
        VkRect2D renderArea = {
            .offset = VkOffset2D {0, 0},
            .extent = extent,
        };

        VkRenderPassBeginInfo renderPassBegin = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = swapchain.GetRenderPass(),
            .framebuffer = swapchain.GetFramebuffer(currentImage),
            .renderArea = renderArea,
            .clearValueCount = 1,
            .pClearValues = &clearValue,
        };

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
            swapchain.GetImageSemaphore(currentImage),
        };

        VkSemaphore renderSemaphore[] = {
            swapchain.GetRenderSemaphore(currentImage),
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

        window.PollEvents();
        window.SwapBuffers();

        currentImage = ++currentImage % swapchain.GetImageCount();
    }
}

Engine::operator VkInstance&()
{
    return instance;
}
Engine::operator VkPhysicalDevice&()
{
    return physicalDevice;
}
Engine::operator VkDevice&()
{
    return device;
}
Engine::operator Window&()
{
    return window;
}
Engine::operator VkSurfaceKHR&()
{
    return surface;
}
Engine::operator Device&()
{
    return device;
}
Engine::operator Swapchain&()
{
    return swapchain;
}
