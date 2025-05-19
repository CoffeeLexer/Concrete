#include "ShaderManager.h"

#include <fstream>
#include <filesystem>
#include <format>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "Scope.h"

struct ShaderStage {
    VkShaderStageFlagBits stage;
    std::vector<uint32_t> spirv;
    VkShaderModule module = VK_NULL_HANDLE;
};

VkShaderModule createShaderModule(const VkDevice device, std::vector<uint32_t> code)
{
    VkShaderModuleCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size() * sizeof(uint32_t),
        .pCode = code.data(),
    };

    VkShaderModule module;
    VkResult result = vkCreateShaderModule(device, &ci, nullptr, &module);
    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed Shader Module Creation");

    return module;
}

void createShaderModules(const VkDevice device, std::vector<ShaderStage> &stages)
{
    for(auto &stage : stages)
        stage.module = createShaderModule(device, stage.spirv);
}

std::vector<uint32_t> loadSPIRV(std::string file)
{
    if (!std::filesystem::exists(file))
        throw std::runtime_error(std::format("File not found: %s", file));

    std::basic_ifstream<uint32_t> ifs{file, std::ifstream::in | std::ifstream::ate | std::ifstream::binary};
    size_t size = ifs.tellg();
    ifs.seekg(0);
    std::vector<uint32_t> code(size);
    ifs.read(code.data(), size);
    ifs.close();
    return code;
}

Shader ShaderManager::createShader(ShaderSources sources)
{
    std::vector<ShaderStage> stages;

    if (!sources.vertex.empty())
    {
        stages.push_back(ShaderStage {
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .spirv = loadSPIRV(sources.vertex)
        });
    }
    if (!sources.fragment.empty())
    {
        stages.push_back(ShaderStage {
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .spirv = loadSPIRV(sources.fragment)
        });
    }

    const auto device = scope.getDevice().getVkDevice();
    createShaderModules(device, stages);
    std::vector<VkPipelineShaderStageCreateInfo> stageCIs{};
    for (const auto& stage : stages)
    {
        stageCIs.push_back(
            VkPipelineShaderStageCreateInfo {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = stage.stage,
                .module = stage.module,
                .pName = "main",
                .pSpecializationInfo = nullptr,
        });
    }

}

VkPipelineVertexInputStateCreateInfo emptyVertexInputStateCI()
{
     return VkPipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };
}

VkPipelineInputAssemblyStateCreateInfo emptyInputAssemblyStateCI()
{
    return VkPipelineInputAssemblyStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };
}

VkPipelineRasterizationStateCreateInfo emptyRasterizationStateCI()
{
    return VkPipelineRasterizationStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0f,
    };
}

VkPipelineMultisampleStateCreateInfo emptyMultisampleStateCI()
{
    return VkPipelineMultisampleStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 0.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };
}

VkPipelineColorBlendAttachmentState emptyColorBlendAttachmentState()
{
    return VkPipelineColorBlendAttachmentState {
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Default
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Default
        .colorBlendOp = VK_BLEND_OP_ADD, // Default
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Default
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Default
        .alphaBlendOp = VK_BLEND_OP_ADD, // Default
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };
}

Shader ShaderManager::createShader(std::vector<VkPipelineShaderStageCreateInfo> stageCIs)
{
    const auto vertexInputState = emptyVertexInputStateCI();
    const auto inputAssemblyState = emptyInputAssemblyStateCI();

    VkExtent2D extent = {
        .width = (uint32_t)scope.getWindow().getInfo().width,
        .height = (uint32_t)scope.getWindow().getInfo().height,
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    VkRect2D scissor = {
        .offset = VkOffset2D{0, 0},
        .extent = extent,
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    const auto rasterizationState = emptyRasterizationStateCI();
    const auto multisampleState = emptyMultisampleStateCI();
    const auto attachment = emptyColorBlendAttachmentState();

    VkPipelineColorBlendStateCreateInfo colorBlendState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &attachment,
        .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f},
    };

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState),
        .pDynamicStates = dynamicStates,
    };

    VkPipelineLayoutCreateInfo layout_ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VkDevice device = scope.getDevice().getVkDevice();
    VkPipelineLayout pipelineLayout;
    VkResult result = vkCreatePipelineLayout(device, &layout_ci, nullptr, &pipelineLayout);
    if(result != VK_SUCCESS)
        throw std::runtime_error("Failed pipeline layout create");

    VkRenderPass renderPass = scope.getBackbuffer().getVkRenderPass();
    VkGraphicsPipelineCreateInfo pipeline_ci = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = (uint32_t)stageCIs.size(),
        .pStages = stageCIs.data(),
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pTessellationState = nullptr,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendState,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = 0,
    };

    VkPipeline pipeline;
    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_ci, nullptr, &pipeline);
    if (result != VK_SUCCESS)
        throw std::runtime_error("Pipeline create failed");


    for (auto& stageCI : stageCIs)
        vkDestroyShaderModule(device, stageCI.module, nullptr);
}
