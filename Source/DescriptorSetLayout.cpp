#include "DescriptorSetLayout.h"
#include <stdexcept>
#include "Engine.h"

DescriptorSetLayout::DescriptorSetLayout(Engine *engine)
    : Link(engine)
{
    VkDescriptorSetLayoutBinding binding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 0,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    VkDescriptorSetLayoutCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = &binding, // layout (binding=X)
    };

    VkDevice device = Owner().device;
    VkResult result = vkCreateDescriptorSetLayout(device, &ci, nullptr, &handle);
    if (result != VK_SUCCESS)
    {
        panic("Failed Init DescriptorSet");
    }
}
