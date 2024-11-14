#pragma once

#include <vulkan/vulkan.h>

class Engine;

class Image
{
    Engine &engine;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;

    void CreateView();
    void CreateMemory();
public:
    Image(Engine &engine, VkExtent3D extent);
};
