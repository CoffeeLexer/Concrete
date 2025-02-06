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
    VkCommandPool commandPool;
    uint32_t imageCount;
    uint32_t currentImage;

    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkSemaphore> renderSemaphores;
    std::vector<VkSemaphore> imageSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;

    VkPresentModeKHR GetBestPresentMode();
    VkSurfaceFormatKHR GetBestSurfaceFormat();
    VkSurfaceCapabilitiesKHR GetSurfaceCaps();
    void CreateImageViews();
    void CreateFramebuffers();
    void CreateSemaphores();
    void AllocateCommandPool();

public:
    Swapchain(Engine& engine);
    ~Swapchain();
    VkFormat GetFormat();
    VkRenderPass GetRenderPass();
    uint32_t GetImageCount();
    VkFramebuffer GetFramebuffer(uint32_t i);
    operator VkSwapchainKHR&();
    void Draw();
};
