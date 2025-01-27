#pragma once

#include "vulkan/vulkan.h"
class Engine;

class Swapchain
{
    Engine &engine;
    VkSwapchainKHR swapchain;
    VkFormat format;
    VkExtent2D extent;

    VkPresentModeKHR GetBestPresentMode();
    VkSurfaceFormatKHR GetBestSurfaceFormat();
    VkSurfaceCapabilitiesKHR GetSurfaceCaps();

public:
    Swapchain(Engine& engine);
    ~Swapchain();
    VkFormat GetFormat();
    VkExtent2D GetExtent();
};
