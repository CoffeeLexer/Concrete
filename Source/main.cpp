#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "vulkan/vulkan.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#define FATAL(statement) if(!(statement)) {printf("FATAL ERROR %s:%i (%s)\n", __PRETTY_FUNCTION__, __LINE__, #statement); exit(1);}

class Engine;
class Instance;
class Surface;
class Device;
class CommandPool;
class CommandBuffer;

class Surface
{
    const VkInstance &instance;
    const VkSurfaceKHR &surface;
public:
    operator const VkSurfaceKHR&() const
    {
        return surface;
    }
    Surface(const VkInstance &instance, const VkSurfaceKHR &surface)
        : instance(instance)
        , surface(surface) {}

    ~Surface()
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
    }
};

class Instance
{
    friend Engine;
    friend Device;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    static std::vector<const char*> GetRequiredExtensions()
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

    static VkBool32 VKAPI_PTR debugLogCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData)
    {
        fprintf(stderr, "VVL ERROR: %s\n", pCallbackData->pMessage);
        return VK_FALSE;
    }

public:
    operator const VkInstance&() const
    {
        return instance;
    }

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
    ~Instance()
    {
        auto pfnDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        pfnDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        vkDestroyInstance(instance, nullptr);
    }
};

class Device
{
    friend CommandPool;
    friend CommandBuffer;

    const Instance &instance;
    const Surface &surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkPhysicalDevice PickPhysicalDevice()
    {
        uint32_t count;
        vkEnumeratePhysicalDevices(instance, &count, nullptr);
        std::vector<VkPhysicalDevice> devices = {};
        devices.resize(count);
        vkEnumeratePhysicalDevices(instance, &count, devices.data());
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

    std::vector<VkBool32> GetPresentSupportVector()
    {
        uint32_t familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
        std::vector<VkBool32> supportArray;
        supportArray.resize(familyCount);

        for (uint32_t i = 0; i < familyCount; i++)
        {
            VkBool32 supported;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported);
            supportArray[i] = supported;
        }
        return supportArray;
    }

    std::vector<VkQueueFamilyProperties> GetQueueFamilyProperties()
    {
        uint32_t familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, nullptr);
        std::vector<VkQueueFamilyProperties> properties;
        properties.resize(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &familyCount, properties.data());
        return properties;
    }

    std::tuple<uint32_t, uint32_t> PickQueueFamily()
    {
        auto presentSupport = GetPresentSupportVector();
        auto queueFamilyProperties = GetQueueFamilyProperties();

        for (uint32_t i = 0; i < presentSupport.size(); i++)
        {
            if (presentSupport[i] == VK_TRUE)
            {
                const auto& property = queueFamilyProperties.at(i);
                if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                    return {i, i};
            }
        }

        uint32_t graphicsQueue = UINT32_MAX;
        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
        {
            const auto& property = queueFamilyProperties.at(i);

            if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueue = i;
                break;
            }
        }

        if (graphicsQueue == UINT32_MAX)
            throw std::runtime_error("No queue graphics family");

        uint32_t presentQueue = UINT32_MAX;
        for (uint32_t i = 0; i < presentSupport.size(); i++)
        {
            if (presentSupport.at(i) == VK_TRUE)
            {
                presentQueue = i;
                break;
            }
        }

        if (presentQueue == UINT32_MAX)
            throw std::runtime_error("No queue present family");

        return {presentQueue, graphicsQueue};
    }

    std::vector<const char*> GetExtensions()
    {
        return (std::vector<const char*>) {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
    }
public:
    operator const VkDevice&() const
    {
        return device;
    }


    // TODO: Queue Factory or mngr
    Device(const Instance& i, const Surface& s)
        : instance(i)
        , surface(s)
    {
        physicalDevice = PickPhysicalDevice();

        float priorities[] = {1.0f, 1.0f};
        const auto & [presentFamily, graphicsFamily] = PickQueueFamily();

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

    ~Device()
    {
        vkDestroyDevice(device, nullptr);
    }
};

class CommandPool
{
    friend CommandBuffer;

    VkCommandPool pool;
    const Device &device;
public:
    CommandPool(const Device &device)
        : device(device)
    {
        VkCommandPoolCreateInfo ci = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = device.familyIndex,
        };

        VkResult result = vkCreateCommandPool(device, &ci, nullptr, &pool);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Command Pool init failed");
        }
    }

    ~CommandPool()
    {
        vkDestroyCommandPool(device, pool, nullptr);
    }
};

class CommandBuffer
{
    const CommandPool &pool;
    VkCommandBuffer buffer;
public:
    CommandBuffer(const CommandPool &pool)
        : pool(pool)
    {
        VkCommandBufferAllocateInfo ai = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = pool.pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkResult result = vkAllocateCommandBuffers(pool.device, &ai, &buffer);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed allocate cmd buffer");
        }
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

    VkSurfaceKHR CreateSurface(const VkInstance& instance)
    {
        VkSurfaceKHR surface;
        VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Couldn't create surface");
        }
        return surface;
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
    Surface surface = Surface(instance, window.CreateSurface(instance));
    Device device = Device(instance, surface);
    CommandPool pool = CommandPool(device);
    CommandBuffer buffer = CommandBuffer(pool);

    while (window.IsValid())
    {
        window.PollEvents();
        window.SwapBuffers();
    }

    return 0;
}