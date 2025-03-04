#pragma once

#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

class Engine;

class UI {
    Engine &engine;
    VkRenderPass renderPass;
    VkDescriptorPool descriptorPool;
    ImGuiContext* context;
    ImGuiContext* lastContext;
    VkSwapchainKHR lastSwapchain;
    std::vector<VkFramebuffer> framebuffers;
    VkRenderPass CreateRenderPass();
    void DestroyFramebuffers();
    void CreateFramebuffers();

public:
    UI(Engine& engine);
    void Start();
    void End(VkCommandBuffer cmd, VkExtent2D extent);
    VkDescriptorPool CreateDescriptorPool();
};
