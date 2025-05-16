#pragma once

#include "vulkan/vulkan.h"
#include <vector>

struct Frame {
    VkImageView imageView           = VK_NULL_HANDLE;
    VkImage image                   = VK_NULL_HANDLE;
    VkFramebuffer framebuffer       = VK_NULL_HANDLE;
    VkSemaphore renderSemaphore     = VK_NULL_HANDLE;
    VkSemaphore imageSemaphore      = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer   = VK_NULL_HANDLE;
    VkFence renderFence             = VK_NULL_HANDLE;
};

class Scope;
class Backbuffer
{
    Scope &scope;

    VkSwapchainKHR swapchain;
    VkPresentModeKHR presentMode;
    VkRenderPass renderPass;
    VkCommandPool commandPool;
    std::vector<Frame> frames;

    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent = {};
    uint32_t imageCount = 3;
    uint32_t currentImage = 0;

    void selectPresentMode();
    void selectSurfaceFormat();

    void createSwapchain();
    void createImages();
    void createImageViews();
    void createRenderPass();
    void createFramebuffers();
    void createSemaphores();
    void createFences();
    void allocateCommandPool();

    void destroySwapchain();
    void destroyImages();
    void destroyImageViews();
    void destroyRenderPass();
    void destroyFramebuffers();
    void destroySemaphores();
    void deallocateCommandPool();
    void destroyFences();

public:
    Backbuffer(Scope &scope);
    ~Backbuffer();
    Backbuffer(const Backbuffer&) = delete;
    Backbuffer& operator=(const Backbuffer&) = delete;
    void draw();
};
