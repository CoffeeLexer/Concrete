#pragma once

#include "vulkan/vulkan.h"

#include "RenderPass.h"
#include "UI.h"

#include <vector>

class Engine;

class Backbuffer
{
    Engine &engine;

    RenderPass *renderPass;

    VkSwapchainKHR swapchain;
    VkFormat format;
    VkExtent2D extent;
    VkCommandPool commandPool;
    uint32_t imageCount;
    uint32_t currentImage;
    UI ui;

    std::vector<VkImageView> imageViews;
    std::vector<VkImage> images;
    std::vector<VkFramebuffer> framebuffers;
    std::vector<VkSemaphore> renderSemaphores;
    std::vector<VkSemaphore> imageSemaphores;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkFence> renderFences;

    VkPresentModeKHR GetBestPresentMode();
    VkSurfaceFormatKHR GetBestSurfaceFormat();
    VkSurfaceCapabilitiesKHR GetSurfaceCaps();
    void CreateImageViews();
    void CreateFramebuffers();
    void CreateSemaphores();
    void CreateFences();
    void AllocateCommandPool();

public:
    Backbuffer(Engine& engine);
    ~Backbuffer();
    VkFormat GetFormat();
    VkRenderPass GetRenderPass();
    uint32_t GetImageCount();
    VkFramebuffer GetFramebuffer(uint32_t i);
    VkImageView GetView(uint32_t i);
    operator VkSwapchainKHR&();
    operator VkExtent2D&();

    void GetFramebuffer();

    void Draw();
    uint32_t GetIndex();
};
