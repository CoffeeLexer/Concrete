#include "PipelineLayout.h"
#include <stdexcept>
#include "Engine.h"

PipelineLayout::PipelineLayout(Engine *engine)
    : Link(engine)
{
    VkPipelineLayoutCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr, // layout (set=X)
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VkDevice device = Owner().device;
    VkResult result = vkCreatePipelineLayout(device, &ci, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed Creating PipelineLayout");
    }
}

PipelineLayout::~PipelineLayout()
{

}
