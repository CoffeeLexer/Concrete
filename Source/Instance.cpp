#include "Instance.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include <vector>
#include <stdexcept>

namespace {
    VkBool32 VKAPI_PTR debugLogCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        fprintf(stderr, "VVL ERROR: %s\n", pCallbackData->pMessage);
        return VK_FALSE;
    }

    std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        // Win: "VK_KHR_surface", "VK_KHR_win32_surface"

        std::vector<const char*> extensions;
        extensions.resize(glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
            extensions[i] = glfwExtensions[i];

#if __APPLE__
        extensions.push_back(VK_KHR_portability_enumeration);
#endif
        return extensions;
    }
}

Instance::Instance(const Engine &engine)
    : engine(engine)
{
    VkApplicationInfo info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Concrete App",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Concrete Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };

#if __APPLE__
    const VkInstanceCreateFlags flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
    const VkInstanceCreateFlags flags = 0;
#endif
    auto extensions = GetRequiredExtensions();

#if !NDEBUG
    extensions.push_back("VK_EXT_debug_utils");
#endif

    VkInstanceCreateInfo ci = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .pApplicationInfo = &info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = 0,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    VkResult result = vkCreateInstance(&ci, nullptr, &instance);
    if (result != VK_SUCCESS)
        throw std::runtime_error("FAILED INSTANCE");

    VkDebugUtilsMessengerCreateInfoEXT callback = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
        .pfnUserCallback = debugLogCallback,
        .pUserData = nullptr,
    };

#if !NDEBUG
    auto pfnCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    pfnCreateDebugUtilsMessengerEXT(instance, &callback, nullptr, &debugMessenger);
#endif

}

Instance::~Instance()
{
    auto pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    pfnDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    vkDestroyInstance(instance, nullptr);
}