#include "ShaderManager.h"

#include <fstream>
#include <filesystem>
#include <format>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "Scope.h"
#include "Shader.h"

const std::vector<uint32_t> ERROR_VERTEX_CODE = {
    0x07230203,0x00010000,0x000d000b,0x0000002a,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000000,0x00000004,0x6e69616d,0x00000000,0x00000016,0x00000021,0x00030003,
    0x00000002,0x0000014a,0x000a0004,0x475f4c47,0x4c474f4f,0x70635f45,0x74735f70,0x5f656c79,
    0x656e696c,0x7269645f,0x69746365,0x00006576,0x00080004,0x475f4c47,0x4c474f4f,0x6e695f45,
    0x64756c63,0x69645f65,0x74636572,0x00657669,0x00040005,0x00000004,0x6e69616d,0x00000000,
    0x00050005,0x0000000a,0x69736f70,0x6e6f6974,0x00000073,0x00050005,0x00000013,0x69736f70,
    0x6e6f6974,0x00000000,0x00060005,0x00000016,0x565f6c67,0x65747265,0x646e4978,0x00007865,
    0x00060005,0x0000001f,0x505f6c67,0x65567265,0x78657472,0x00000000,0x00060006,0x0000001f,
    0x00000000,0x505f6c67,0x7469736f,0x006e6f69,0x00070006,0x0000001f,0x00000001,0x505f6c67,
    0x746e696f,0x657a6953,0x00000000,0x00070006,0x0000001f,0x00000002,0x435f6c67,0x4470696c,
    0x61747369,0x0065636e,0x00030005,0x00000021,0x00000000,0x00040047,0x00000016,0x0000000b,
    0x0000002a,0x00050048,0x0000001f,0x00000000,0x0000000b,0x00000000,0x00050048,0x0000001f,
    0x00000001,0x0000000b,0x00000001,0x00050048,0x0000001f,0x00000002,0x0000000b,0x00000003,
    0x00030047,0x0000001f,0x00000002,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,
    0x00030016,0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000002,0x00040018,
    0x00000008,0x00000007,0x00000003,0x00040020,0x00000009,0x00000006,0x00000008,0x0004003b,
    0x00000009,0x0000000a,0x00000006,0x0004002b,0x00000006,0x0000000b,0x00000000,0x0004002b,
    0x00000006,0x0000000c,0x3f000000,0x0005002c,0x00000007,0x0000000d,0x0000000b,0x0000000c,
    0x0004002b,0x00000006,0x0000000e,0xbf000000,0x0005002c,0x00000007,0x0000000f,0x0000000c,
    0x0000000e,0x0005002c,0x00000007,0x00000010,0x0000000e,0x0000000e,0x0006002c,0x00000008,
    0x00000011,0x0000000d,0x0000000f,0x00000010,0x00040020,0x00000012,0x00000007,0x00000007,
    0x00040015,0x00000014,0x00000020,0x00000001,0x00040020,0x00000015,0x00000001,0x00000014,
    0x0004003b,0x00000015,0x00000016,0x00000001,0x00040020,0x00000018,0x00000006,0x00000007,
    0x00040017,0x0000001b,0x00000006,0x00000004,0x00040015,0x0000001c,0x00000020,0x00000000,
    0x0004002b,0x0000001c,0x0000001d,0x00000001,0x0004001c,0x0000001e,0x00000006,0x0000001d,
    0x0005001e,0x0000001f,0x0000001b,0x00000006,0x0000001e,0x00040020,0x00000020,0x00000003,
    0x0000001f,0x0004003b,0x00000020,0x00000021,0x00000003,0x0004002b,0x00000014,0x00000022,
    0x00000000,0x0004002b,0x00000006,0x00000024,0x3f800000,0x00040020,0x00000028,0x00000003,
    0x0000001b,0x00050036,0x00000002,0x00000004,0x00000000,0x00000003,0x000200f8,0x00000005,
    0x0004003b,0x00000012,0x00000013,0x00000007,0x0003003e,0x0000000a,0x00000011,0x0004003d,
    0x00000014,0x00000017,0x00000016,0x00050041,0x00000018,0x00000019,0x0000000a,0x00000017,
    0x0004003d,0x00000007,0x0000001a,0x00000019,0x0003003e,0x00000013,0x0000001a,0x0004003d,
    0x00000007,0x00000023,0x00000013,0x00050051,0x00000006,0x00000025,0x00000023,0x00000000,
    0x00050051,0x00000006,0x00000026,0x00000023,0x00000001,0x00070050,0x0000001b,0x00000027,
    0x00000025,0x00000026,0x0000000b,0x00000024,0x00050041,0x00000028,0x00000029,0x00000021,
    0x00000022,0x0003003e,0x00000029,0x00000027,0x000100fd,0x00010038
};

const std::vector<uint32_t> ERROR_FRAGMENT_CODE = {
    0x07230203,0x00010000,0x000d000b,0x0000000d,0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0006000f,0x00000004,0x00000004,0x6e69616d,0x00000000,0x00000009,0x00030010,0x00000004,
    0x00000007,0x00030003,0x00000002,0x0000014a,0x000a0004,0x475f4c47,0x4c474f4f,0x70635f45,
    0x74735f70,0x5f656c79,0x656e696c,0x7269645f,0x69746365,0x00006576,0x00080004,0x475f4c47,
    0x4c474f4f,0x6e695f45,0x64756c63,0x69645f65,0x74636572,0x00657669,0x00040005,0x00000004,
    0x6e69616d,0x00000000,0x00040005,0x00000009,0x6f6c6f63,0x00000072,0x00040047,0x00000009,
    0x0000001e,0x00000000,0x00020013,0x00000002,0x00030021,0x00000003,0x00000002,0x00030016,
    0x00000006,0x00000020,0x00040017,0x00000007,0x00000006,0x00000004,0x00040020,0x00000008,
    0x00000003,0x00000007,0x0004003b,0x00000008,0x00000009,0x00000003,0x0004002b,0x00000006,
    0x0000000a,0x00000000,0x0004002b,0x00000006,0x0000000b,0x3f400000,0x0007002c,0x00000007,
    0x0000000c,0x0000000a,0x0000000a,0x0000000b,0x0000000a,0x00050036,0x00000002,0x00000004,
    0x00000000,0x00000003,0x000200f8,0x00000005,0x0003003e,0x00000009,0x0000000c,0x000100fd,
    0x00010038
};

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

Shader ShaderManager::createShader(ShaderSources sources)
{
    std::vector<ShaderStage> stages;

    if (!sources.vertex.empty())
    {
        stages.push_back(ShaderStage {
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .spirv = loadSPIRV(sources.vertex),
        });
    }
    else
    {
        stages.push_back(ShaderStage {
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .spirv = ERROR_VERTEX_CODE,
        });
    }

    if (!sources.fragment.empty())
    {
        stages.push_back(ShaderStage {
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .spirv = loadSPIRV(sources.fragment),
        });
    }
    else
    {
        stages.push_back(ShaderStage {
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .spirv = ERROR_FRAGMENT_CODE,
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

    VkPipelineLayout layout;
    VkResult result = vkCreatePipelineLayout(device, &layout_ci, nullptr, &layout);
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
        .layout = layout,
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

    return Shader(scope, layout, pipeline);
}
