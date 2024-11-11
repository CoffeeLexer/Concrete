#pragma once

#include "vulkan/vulkan.h"
class Engine;

class Swapchain
{
    Engine &engine;
    VkSwapchainKHR swapchain;

    VkPresentModeKHR GetBestPresentMode();
    VkSurfaceFormatKHR GetBestSurfaceFormat();
    VkSurfaceCapabilitiesKHR GetSurfaceCaps();

public:
    Swapchain(Engine& engine);
    ~Swapchain();
};
