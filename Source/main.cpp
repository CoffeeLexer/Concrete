#include <bit>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

class Engine;
class Instance;
class Device;

class Instance
{
    friend Engine;
    friend Device;

    VkInstance instance;

    static std::vector<const char*> GetRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions;
        extensions.resize(glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
            extensions[i] = glfwExtensions[i];

#if __APPLE__
        extensions.push_back(VK_KHR_portability_enumeration);
#endif
        return extensions;
    }
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

#if __APPLE__
        const VkInstanceCreateFlags flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#else
        const VkInstanceCreateFlags flags = 0;
#endif
        auto extensions = Instance::GetRequiredExtensions();

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
    }
    ~Instance()
    {
        vkDestroyInstance(instance, nullptr);
    }
};

class QueueCreateManager
{
    std::unordered_map<VkQueueFlagBits, uint32_t> requests;

    inline void Add(VkQueueFlagBits bit)
    {
        auto item = requests.find(bit);
        if (item == requests.end())
            requests[bit] = 0;
        requests[bit] += 1;
    }
public:
    QueueCreateManager() = default;

    void Request(VkQueueFlagBits flag)
    {
        switch (flag)
        {
        case VK_QUEUE_GRAPHICS_BIT:
        case VK_QUEUE_COMPUTE_BIT:
        case VK_QUEUE_TRANSFER_BIT:
            Add(flag);
            return;
        default:
            fprintf(stderr, "Unrecognized queue type\n");
        }

        return;
    }

    void Submit(VkPhysicalDevice phyDevice)
    {
        uint32_t familyCount;
        VkQueueFamilyProperties *properties;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, properties);

        
        // VkDeviceQueueCreateInfo ci = {
        //     .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        //     .pNext = nullptr,
        //     .flags = 0,
        //     .queueFamilyIndex = ,
        //     .queueCount = ,
        //     .pQueuePriorities = ,
        // };
    }
};

class Queue
{
    friend QueueCreateManager;

    VkQueue queue;


    Queue() = delete;
public:
};

class Device
{
    Instance &instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;


    VkPhysicalDevice PickPhysicalDevice()
    {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance.instance, &count, nullptr);
        std::vector<VkPhysicalDevice> devices = {};
        devices.resize(count);
        vkEnumeratePhysicalDevices(instance.instance, &count, devices.data());
        if (count == 1)
            return devices.at(0);

        for (const auto& device : devices)
        {
            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(device, &features);
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(device, &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return device;
        }

        return devices.at(0);
    }

    uint32_t PickQueueFamily()
    {
        uint32_t familyCount;
        VkQueueFamilyProperties *properties;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, properties);

        for (uint32_t i = 0; i < familyCount; i++)
        {
            const auto& property = properties[i];

            if (property.queueFlags | VK_QUEUE_GRAPHICS_BIT)
                return i;
        }
    }
public:
    Device(Instance i)
        : instance(i)
    {
        physicalDevice = PickPhysicalDevice();

        VkDeviceCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = 0,
            .pQueueCreateInfos = nullptr,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = 0,
            .ppEnabledExtensionNames = nullptr,
            .pEnabledFeatures = nullptr,
        };

        VkResult result = vkCreateDevice(physicalDevice, &ci, nullptr, &device);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed device create");
        }
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

    Engine engine = Engine();
    Instance instance = Instance();
    Device device = Device(instance);

    while (window.IsValid())
    {
        window.PollEvents();
        window.SwapBuffers();
    }

    return 0;
}