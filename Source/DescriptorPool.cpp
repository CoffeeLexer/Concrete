#include "DescriptorPool.h"

#include <stdexcept>

#include "Engine.h"

DescriptorPool::DescriptorPool(Engine &engine)
    : engine(engine)
{
    VkDescriptorPoolSize sizes[1] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
        },
    };

    VkDescriptorPoolCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = sizes,
    };

    VkDevice device = engine;
    VkResult result = vkCreateDescriptorPool(device, &ci, nullptr, &descriptionPool);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed descriptor pool create");
    }

    VkDescriptorPool &descriptorPool = engine;
    VkDescriptorSetLayout &descriptorSetLayout = engine;
    VkDescriptorSetAllocateInfo ai[1] = {
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = ,
            .pSetLayouts = ,
        },
    };
}
