// #include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

#include <cstdint>
#include <cstdlib>
#include <cstring>

const u_int32_t WIDTH = 800;
const u_int32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreatDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

class HelloTriangelApplication {

  public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

  private:
    GLFWwindow* window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
    };

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
#if !defined(NDEBUG)
        printDebugEnabled();
#endif
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error(
                "validationlayers requested, but not available!\n");
        }

        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();

        checkExtesionSupport(static_cast<uint32_t>(extensions.size()),
                             extensions.data());

#ifdef __APPLE__ // for MacOS MoltenVK sdk
        std::vector<const char*> requiredExtentions;

        for (uint32_t i = 0; i < static_cast<uint32_t>(extensions.size());
             i++) {
            requiredExtentions.emplace_back(extensions[i]);
        }

        requiredExtentions.emplace_back(
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        createInfo.enabledExtensionCount = (uint32_t)requiredExtentions.size();
        createInfo.ppEnabledExtensionNames = requiredExtentions.data();

#elif _WIN32 || _WIN64 || linux // #else
        createInfo.enabledExtensionCount =
            static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
#endif

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount =
                static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext =
                (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    void populateDebugMessengerCreateInfo(
        VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        // createInfo.pUserData = nullptr;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreatDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                        &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error(
                "failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        // VkPhysicalDeviceProperties deviceProperties;
        // VkPhysicalDeviceFeatures deviceFeatures;
        // vkGetPhysicalDeviceProperties(device, &deviceProperties);
        // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // printPhysicalDeviceInfo(device, &deviceProperties, &deviceFeatures);

        // return (deviceProperties.deviceType ==
        //             VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
        //         deviceProperties.deviceType ==
        //             VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) &&
        //        deviceFeatures.geometryShader;
        return true;
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;
        return indices;
    }

    void printPhysicalDeviceInfo(VkPhysicalDevice device,
                                 VkPhysicalDeviceProperties* pProperties,
                                 VkPhysicalDeviceFeatures* pFeatures) {
        std::cout << std::endl;
        std::cout << "Device Name: " << pProperties->deviceName << std::endl;
        std::cout << "Device Type: " << pProperties->deviceType << std::endl;
        std::cout << "Device Driver Version: " << pProperties->driverVersion
                  << std::endl;
    }

    void checkExtesionSupport(uint32_t glfwExtensionCount,
                              const char** glfwExtensions) {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               extensions.data());

        std::cout << "\nSupported Extensions (" << extensionCount
                  << "):" << std::endl;
        std::vector<const char*> exist;
        std::vector<const char*> notExist;
        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << std::endl;
            bool found = false;

            for (uint32_t i = 0; i < glfwExtensionCount; i++) {
                if (strcmp(glfwExtensions[i], extension.extensionName) == 0) {
                    exist.emplace_back(glfwExtensions[i]);
                    found = true;
                    break;
                }
            }

            if (!found) {
                notExist.emplace_back(extension.extensionName);
            }
        }

        std::cout << "\nExist GLFW Extensions in Vulkan (" << exist.size()
                  << "):" << std::endl;
        for (auto e : exist) {
            std::cout << '\t' << e << std::endl;
        }
        std::cout << "\nNot Exist GLFW Extensions in Vulkan ("
                  << notExist.size() << "):" << std::endl;
        for (auto n : notExist) {
            std::cout << '\t' << n << std::endl;
        }
    }

    void printDebugEnabled() {
        std::cout << "debug mode enabled..." << "\n"
                  << "enableValidationLayers: " << enableValidationLayers
                  << std::endl;
    }

    void suckMyDick() { std::cout << "\nsuck my dick!!!\n" << std::endl; }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(
            glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageServerity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                  void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage
                  << std::endl;

        return VK_FALSE;
    }
};

int main() {
    HelloTriangelApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
