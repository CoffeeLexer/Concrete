#include "Device.h"

#include <vector>

    Device::Device(const Instance& i, const Surface& s)
        : instance(i)
        , surface(s)
    {
        physicalDevice = PickPhysicalDevice();

        float priorities[] = {1.0f, 1.0f};
        const auto & [presentFamily, graphicsFamily] = PickQueueFamily();
        graphicsIndex = graphicsFamily;
        presentIndex = presentFamily;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};

        if (presentFamily == graphicsFamily)
        {
            VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = presentFamily,
                .queueCount = 2,
                .pQueuePriorities = priorities,
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }
        else
        {
            VkDeviceQueueCreateInfo graphicsQueueInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = graphicsFamily,
                .queueCount = 1,
                .pQueuePriorities = priorities,
            };
            VkDeviceQueueCreateInfo presentQueueInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = presentFamily,
                .queueCount = 1,
                .pQueuePriorities = priorities,
            };
            queueCreateInfos.push_back(presentQueueInfo);
        }

        auto extensions = GetExtensions();
        VkDeviceCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
            .pEnabledFeatures = nullptr,
        };

        VkResult result = vkCreateDevice(physicalDevice, &ci, nullptr, &device);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed device create");
        }
        vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);
    }

    VkPresentModeKHR Device::GetBestPresentMode()
    {
        uint32_t count;
        const VkSurfaceKHR &surface = engine.surface;
        const VkPhysicalDevice &physicalDevice = engine.physicalDevice;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
        std::vector<VkPresentModeKHR> modes = {};
        modes.resize(count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, modes.data());

        VkPresentModeKHR priorities[] = {
            VK_PRESENT_MODE_FIFO_KHR,
            VK_PRESENT_MODE_MAILBOX_KHR,
            VK_PRESENT_MODE_FIFO_RELAXED_KHR,
            VK_PRESENT_MODE_IMMEDIATE_KHR,
        };

        for (const auto& p : priorities)
        {
            for (uint32_t i = 0; i < count; i++)
            {
                if (p == modes.at(i))
                    return p;
            }
        }
        return modes.at(0);
    }


    Device::~Device()
    {
        vkDestroyDevice(device, nullptr);
    }