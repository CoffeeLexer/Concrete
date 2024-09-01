#include <cstdio>
#include <cstdint>
#include <stdexcept>
#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"

class Instance
{
    VkInstance instance;
public:
    Instance()
    {
        VkApplicationInfo info = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Concrete App",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Concrete Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0,
        };

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &info,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = 0,
            .enabledExtensionCount = glfwExtensionCount,
            .ppEnabledExtensionNames = glfwExtensions,
        };

        VkResult result = vkCreateInstance(&ci, nullptr, &instance);
        if (result != VK_SUCCESS)
            throw std::runtime_error("FAILED INSTANCE");
    }
};

class Engine
{

public:

};


int main()
{
    return 0;
}