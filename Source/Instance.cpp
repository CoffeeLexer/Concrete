#include "Instance.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <vector>
#include <stdexcept>

namespace {
    constexpr std::vector<const char*> GetRequiredExtensions()
    {
        return {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if __APPLE__
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
#elif _WIN32
            "VK_KHR_win32_surface",
#endif
#if !NDEBUG
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
#endif
        };
    }
}

void Instance::Create()
{
#if __APPLE__
    constexpr VkInstanceCreateFlags flags =
        VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
    constexpr VkInstanceCreateFlags flags = 0;
#endif

    constexpr auto extensions = GetRequiredExtensions();

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

    if (vkCreateInstance(&ci, nullptr, &handle) != VK_SUCCESS)
        throw std::runtime_error("FAILED INSTANCE");
}

Instance::~Instance()
{
    vkDestroyInstance(handle, nullptr);
}
