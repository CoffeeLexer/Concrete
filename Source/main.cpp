#include <cstdio>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <string>
#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_NONE
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

        std::vector<std::string> extensions;
        extensions.resize(glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
            extensions.push_back(glfwExtensions[i]);

#if __APPLE__
        extensions.push_back(VK_KHR_portability_enumeration);
        const VkInstanceCreateFlags flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
        const VkInstanceCreateFlags flags = 0;
#endif

        VkInstanceCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = flags,
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
    ~Instance()
    {
        vkDestroyInstance(instance, nullptr);
    }
};

class Device
{
    VkPhysicalDevice physicalDevice;
    VkDevice device;
public:
    Device()
    {

    }

    ~Device()
    {

    }
};

class Window
{
    GLFWwindow *window;

    static void CallbackError(int err, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void CallbackKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    inline static uint32_t globalInstanceCount = 0;
    static void GlobalInit()
    {
        globalInstanceCount++;
        if (globalInstanceCount > 1) return;

        if (!glfwInit())
        {
            printf("Failed GLFW INIT");
            return;
        }

        glfwSetErrorCallback(Window::CallbackError);
    }

    static void GlobalTerminate()
    {
        globalInstanceCount--;

        if (globalInstanceCount > 0) return;

        glfwTerminate();
    }

public:

    Window()
    {
        Window::GlobalInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(480, 480, "Concrete Window", nullptr, nullptr);

        if (!window)
        {
            fprintf(stderr, "Failed GLFWwindow creation\n");
            return;
        }

        glfwSetKeyCallback(window, Window::CallbackKey);
    }
    ~Window()
    {
        glfwDestroyWindow(window);
        Window::GlobalTerminate();
    }

    void SwapBuffers()
    {
        //glfwSwapBuffers(window);
    }

    void PollEvents()
    {
        glfwPollEvents();
    }

    bool IsValid()
    {
        return !glfwWindowShouldClose(window);
    }
};

class Engine
{

public:

};

int main()
{
    Window window = Window();

    while (window.IsValid())
    {
        window.PollEvents();
        window.SwapBuffers();
    }

    return 0;
}