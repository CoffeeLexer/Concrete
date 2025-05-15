#pragma once

#include "vulkan/vulkan.h"
#include "Handle.h"
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

    VkSurfaceKHR surface;

    VkSwapchainKHR swapchain;
    VkPresentModeKHR presentMode;

    void selectPresentMode();
    void selectSurfaceFormat();

    VkSurfaceFormatKHR surfaceFormat;

    VkExtent2D extent;
    VkCommandPool commandPool;
    uint32_t imageCount;
    uint32_t currentImage;

    std::vector<Frame> frames;

    void createSurface();
    void createSwapchain();
    void createImageViews();
    void createFramebuffers();
    void createSemaphores();
    void createFences();
    void allocateCommandPool();

public:
    Backbuffer(Scope &scope);
    ~Backbuffer();

    Handle<VkSwapchainKHR> getSwapchain{swapchain};
    Handle<VkSurfaceKHR> getSurface{surface};
};
