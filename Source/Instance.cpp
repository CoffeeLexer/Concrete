#include "Instance.h"
#include "Panic.h"

#include <vector>

std::vector<const char*> GetRequiredExtensions()
{
    std::vector<const char*> extensions;

    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    if constexpr (APPLE)
        extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    if constexpr (WINDOWS)
        extensions.push_back("VK_KHR_win32_surface");

    return extensions;
}

Instance::Instance(Scope &scope) : scope(scope)
{
    constexpr VkInstanceCreateFlags flags = APPLE
    ? VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR
    : 0;

    auto extensions = GetRequiredExtensions();

    constexpr VkApplicationInfo info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Concrete App",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Concrete Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };
    const VkInstanceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .pApplicationInfo = &info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    if (vkCreateInstance(&ci, nullptr, &instance) != VK_SUCCESS)
        panic("Failed Instance Creation");
}

Instance::~Instance()
{
    vkDestroyInstance(instance, nullptr);
    instance = VK_NULL_HANDLE;
}
