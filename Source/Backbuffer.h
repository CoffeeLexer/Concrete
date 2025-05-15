#pragma once

#include "vulkan/vulkan.h"
#include "Handle.h"
#include "RenderPass.h"
#include <vector>

class Scope;
class Backbuffer
{
    Scope &scope;

    VkSurfaceKHR surface;
    void createSurface();
    void createSwapchain();

    VkSwapchainKHR swapchain;
    VkPresentModeKHR presentMode;
    RenderPass *renderPass;

    void selectPresentMode();


    VkFormat format;
    VkColorSpaceKHR colorSpace;

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
    Backbuffer(Scope &scope);
    ~Backbuffer();

        Handle<VkSwapchainKHR> getSwapchain{swapchain};
    Handle<VkSurfaceKHR> getSurface{surface};

    VkFormat GetFormat();
    VkRenderPass GetRenderPass();
    uint32_t GetImageCount();
    VkFramebuffer GetFramebuffer(uint32_t i);
    VkImageView GetView(uint32_t i);

    void GetFramebuffer();
    VkExtent2D GetExtent();
    void Draw();
    uint32_t GetIndex();
};
