#pragma once

#include "vulkan/vulkan.h"

#include "Handle.h"
#include "ScopeLink.h"
#include "Scope.h"
#include "RenderPass.h"

#include <vector>

class Backbuffer
    : public Handle<VkSwapchainKHR>
{
    friend Scope::Scope(), Scope::~Scope();
    ScopeLink scope;

    explicit Backbuffer(Scope *);
    ~Backbuffer();


    RenderPass *renderPass;

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
