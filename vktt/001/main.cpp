// #include <vulkan/vulkan.h>
#include <cstdint>
#include <cstring>
#include <sys/types.h>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

const u_int32_t WIDTH = 800;
const u_int32_t HEIGHT = 600;

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

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() { createInstance(); }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
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

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        checkExtesionSupport(glfwExtensionCount, glfwExtensions);

#ifdef __APPLE__
        // for MoltenVK sdk
        std::vector<const char*> requiredExtentions;

        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            requiredExtentions.emplace_back(glfwExtensions[i]);
        }

        requiredExtentions.emplace_back(
            VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        createInfo.enabledExtensionCount = (uint32_t)requiredExtentions.size();
        createInfo.ppEnabledExtensionNames = requiredExtentions.data();

// #elif _WIN32 || _WIN64 || linux
#else
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create instance!");
        }
    }

    void checkExtesionSupport(uint32_t glfwExtensionCount,
                              const char** glfwExtensions) {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                               extensions.data());

        std::cout << "Supported Extensions (" << extensionCount
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

        std::cout << "Exist GLFW Extensions in Vulkan (" << exist.size()
                  << "):" << std::endl;
        for (auto e : exist) {
            std::cout << '\t' << e << std::endl;
        }
        std::cout << "Not Exist GLFW Extensions in Vulkan (" << notExist.size()
                  << "):" << std::endl;
        for (auto n : notExist) {
            std::cout << '\t' << n << std::endl;
        }
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
