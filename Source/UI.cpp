#include "UI.h"

#include <Engine.h>

UI::UI(Engine &engine)
    : engine(engine)
    , descriptorPool(CreateDescriptorPool())
    , lastSwapchain(VK_NULL_HANDLE)
    , renderPass(CreateRenderPass())
{
    VkInstance &instance = engine;
    VkPhysicalDevice &physicalDevice = engine;
    Device &device = engine;

    IMGUI_CHECKVERSION();
    context = ImGui::CreateContext();
    lastContext = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(context);

    auto& io = ImGui::GetIO();

    Window &window = engine;
    ImGui_ImplGlfw_InitForVulkan(window, true);


    ImGui_ImplVulkan_InitInfo initInfo = {
        .ApiVersion = VK_API_VERSION_1_0,
        .Instance = instance,
        .PhysicalDevice = physicalDevice,
        .Device = device,
        .QueueFamily = device.GetGraphicsIndex(),
        .Queue = device.GetGraphicsQueue(),
        .DescriptorPool = descriptorPool,
        .RenderPass = renderPass,
        .MinImageCount = 3,
        .ImageCount = 3,
        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
    };

    ImGui_ImplVulkan_Init(&initInfo);
    ImGui::SetCurrentContext(lastContext);
}

void UI::Start()
{
    Backbuffer &backbuffer = engine;
    VkSwapchainKHR swapchain = backbuffer;
    if (lastSwapchain != swapchain)
    {
        lastSwapchain = swapchain;
        DestroyFramebuffers();
        CreateFramebuffers();
    }

    lastContext = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(context);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UI::End(VkCommandBuffer cmd, VkExtent2D extent)
{
    ImGui::Render();

    VkDevice device = engine;
    Backbuffer &backbuffer = engine;
    VkSwapchainKHR &current = backbuffer;

    VkRenderPassBeginInfo bi = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = framebuffers[backbuffer.GetIndex()],
        .renderArea = VkRect2D {
            .offset = VkOffset2D { .x = 0, .y = 0 },
            .extent = extent,
        },
        .clearValueCount = 0,
        .pClearValues = nullptr,
    };

    vkCmdBeginRenderPass(cmd, &bi, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    vkCmdEndRenderPass(cmd);

    ImGui::SetCurrentContext(lastContext);
}

VkDescriptorPool UI::CreateDescriptorPool()
{
    constexpr uint32_t maxSamples = 65'536;
    const VkDevice &device = engine;

    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = maxSamples,
    };

    const VkDescriptorPoolCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = maxSamples,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(device, &ci, nullptr, &descriptorPool);
    return descriptorPool;
}

VkRenderPass UI::CreateRenderPass()
{
    const VkDevice &device = engine;
    Backbuffer &backbuffer = engine;

    VkAttachmentDescription attachment_description = {
        .flags = 0,
        .format = backbuffer.GetFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference attachmennt = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass_description = {
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmennt,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dependencyFlags = 0,
    };

    VkRenderPassCreateInfo renderPass_ci = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &attachment_description,
        .subpassCount = 1,
        .pSubpasses = &subpass_description,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    VkRenderPass renderPass;
    vkCreateRenderPass(device, &renderPass_ci, nullptr, &renderPass);
    return renderPass;
}

void UI::DestroyFramebuffers()
{
    if (lastSwapchain == VK_NULL_HANDLE) return;
    const VkDevice &device = engine;
    for (const auto& framebuffer : framebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
}

void UI::CreateFramebuffers()
{
    const VkDevice &device = engine;
    Backbuffer &backbuffer = engine;
    VkExtent2D extent = backbuffer;

    framebuffers.resize(backbuffer.GetImageCount());

    for (uint32_t i = 0; i < backbuffer.GetImageCount(); i++)
    {
        VkImageView attachments[] = {
            backbuffer.GetView(i),
        };
        VkFramebufferCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = attachments,
            .width = extent.width,
            .height = extent.height,
            .layers = 1,
        };

        vkCreateFramebuffer(device, &ci, nullptr, &framebuffers[i]);
    }
}
