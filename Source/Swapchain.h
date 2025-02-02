#pragma once

#include "vulkan/vulkan.h"

#include "RenderPass.h"

#include <vector>

class Engine;

class Swapchain
{
    Engine &engine;

    RenderPass *renderPass;

    VkSwapchainKHR swapchain;
    VkFormat format;
    VkExtent2D extent;
    uint32_t imageCount;

    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkSemaphore> renderSemaphores;
    std::vector<VkSemaphore> imageSemaphores;

    VkPresentModeKHR GetBestPresentMode();
    VkSurfaceFormatKHR GetBestSurfaceFormat();
    VkSurfaceCapabilitiesKHR GetSurfaceCaps();
    void CreateImageViews();
    void CreateFramebuffers();
    void CreateSemaphores();

public:
    Swapchain(Engine& engine);
    ~Swapchain();
    VkFormat GetFormat();
    VkExtent2D GetExtent();
    VkRenderPass GetRenderPass();
    uint32_t GetImageCount();
    VkFramebuffer GetFramebuffer(uint32_t i);
    VkSemaphore GetRenderSemaphore(uint32_t i);
    VkSemaphore GetImageSemaphore(uint32_t i);
    operator VkSwapchainKHR&();
};
