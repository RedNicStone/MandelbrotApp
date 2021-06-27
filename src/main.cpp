#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <bitset>
#include <chrono>
#include <cmath>

#include "utils.h"
#include "saved_view.h"

const uint32_t WIDTH = 1080;
const uint32_t HEIGHT = 720;

const long double ZOOM_STEP = 1.1L;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const uint32_t debugLevel = 3;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    
    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct MandelbrotUBO {
    glm::highp_vec2 pos;
    glm::mediump_ivec2 res;
    double zoom;
    uint16_t iterations;
    uint16_t color;
    
    double time;
};

class MandelbrotApp {
  public:
    void run() {
        std::cout << "starting application\n";
        
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
  
  private:
    GLFWwindow *window;
    int windowWidth = WIDTH;
    int windowHeight = HEIGHT;
    
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFrameBuffers;
    
    VkRenderPass renderPass;
    VkDescriptorSetLayout uboDescriptorSetLayout;
    
    VkPipelineLayout graphicsPipelineLayout;
    VkPipeline graphicsPipeline;
    
    VkCommandPool commandPool;
    
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    
    std::vector<VkCommandBuffer> graphicsCommandBuffers;
    
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    std::array<float, 60> lastFrameDeltas;
    size_t currentFrame = 0;
    float currentFPS = 0;
    
    bool framebufferResized = false;
    
    VkDescriptorPool graphicsDescriptorPool;
    std::vector<VkDescriptorSet> graphicsDescriptorSets;
    
    VkDescriptorPool imguiDescriptorPool;
    
    long double zoomScale = 3.5L; //1.7e-10;
    long double posReal = -2.5L; //-0.04144230656908739;
    long double posImg = -1.75L; //1.48014290228390966;
    
    const bool enableVSync = false;
    bool autoMaxIterations = true;
    int maxIterations = 300;
    bool ImGuiEnabled = true;
    int color = 0;
    
    void initWindow() {
        if (debugLevel > 0) { std::cout << "initializing window\n"; }
        
        glfwInit();
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwSetErrorCallback(GLFWErrorCallback);
    
        window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, GLFWFramebufferResizeCallback);
        glfwSetScrollCallback(window, GLFWMouseScrollCallback);
        glfwSetKeyCallback(window, GLFWKeyCallback);
        if (enableVSync) glfwSwapInterval(1);
    }
    
    static void GLFWFramebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto *app = reinterpret_cast<MandelbrotApp *>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }
    
    static void GLFWErrorCallback(int error, const char* description) {
        std::cout << "GLFW error: (" << error << "): " << description << std::endl;
    }
    
    static void GLFWMouseScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
        auto *app = reinterpret_cast<MandelbrotApp *>(glfwGetWindowUserPointer(window));
        if (yOffset == 1.0)
            app->zoom(1 / ZOOM_STEP);
        else if (yOffset == -1.0)
            app->zoom(ZOOM_STEP);
    }
    
    static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto *app = reinterpret_cast<MandelbrotApp *>(glfwGetWindowUserPointer(window));
        switch (key) {
            case GLFW_KEY_PAUSE:
                if (action == GLFW_PRESS)
                    glfwSetWindowShouldClose(window, true);
                break;
            
            case GLFW_KEY_ESCAPE:
                if (action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                    glfwSetWindowShouldClose(window, true);
                break;
            
            case GLFW_KEY_ENTER:
                if (action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                    app->toggleImGUI();
                break;
        }
    }
    
    void initVulkan() {
        if (debugLevel > 0) { std::cout << "initializing vulkan\n"; }
        
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDescriptorSetLayout();
        createGraphicsPipeline();
        createFrameBuffers();
        createCommandPool();
        createUniformBuffers();
        createGraphicsDescriptorPools();
        createGraphicsDescriptorSets();
        createGraphicsCommandBuffers();
        createSyncObjects();
        //createImGUIDescriptorPools();
        //initImGUI();
    }
    
    void mainLoop() {
        if (debugLevel > 0) { std::cout << "entering main loop\n"; }
        
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            
            //ImGui_ImplVulkan_NewFrame();
            
            //ImGui_ImplGlfw_NewFrame();
    
            //ImGui::NewFrame();
            
            //ImGui::ShowDemoWindow();
            //setupImGUIFrame();
            drawFrame();
        }
        
        vkDeviceWaitIdle(device);
        
        if (debugLevel > 0) { std::cout << "exiting\n"; }
    }
    
    void cleanupSwapChain() {
        for (auto framebuffer : swapChainFrameBuffers) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        
        vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(graphicsCommandBuffers.size()),
                             graphicsCommandBuffers.data());
        
        vkDestroyPipeline(device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device, graphicsPipelineLayout, nullptr);
        vkDestroyRenderPass(device, renderPass, nullptr);
        
        for (auto imageView : swapChainImageViews) {
            vkDestroyImageView(device, imageView, nullptr);
            
        }
        
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            vkDestroyBuffer(device, uniformBuffers[i], nullptr);
            vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
        }
        
        vkDestroyDescriptorPool(device, graphicsDescriptorPool, nullptr);
    }
    
    void cleanup() {
        if (debugLevel > 0) { std::cout << "cleaning up\n"; }
        
        cleanupSwapChain();
        
        vkDestroyDescriptorSetLayout(device, uboDescriptorSetLayout, nullptr);
        
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(device, inFlightFences[i], nullptr);
        }
        
        vkDestroyCommandPool(device, commandPool, nullptr);
        
        vkDestroyDevice(device, nullptr);
        
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
        
        glfwDestroyWindow(window);
        
        glfwTerminate();
    }
    
    void recreateSwapChain() {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
        
        vkDeviceWaitIdle(device);
        
        cleanupSwapChain();
        
        createSwapChain();
        createImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createFrameBuffers();
        createUniformBuffers();
        createGraphicsDescriptorPools();
        createGraphicsDescriptorSets();
        createGraphicsCommandBuffers();
        
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
    }
    
    void createInstance() {
        if (debugLevel > 0) { std::cout << "creating vulkan instance\n"; }
        
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
        
        if (!checkExtensionSupport()) {
            throw std::runtime_error("not all required extensions available");
        }
        
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "MandelbrotProject";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
            
            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;
            
            createInfo.pNext = nullptr;
        }
        
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to crate vulkan instance!");
        }
    }
    
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }
    
    void setupDebugMessenger() {
        if (!enableValidationLayers) return;
        
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);
        
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    
    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }
    
    void pickPhysicalDevice() {
        
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        
        if (debugLevel > 0) std::cout << "\n\nFound " << deviceCount << " device[s]\n";
        
        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }
        
        std::vector<VkPhysicalDevice> possibleDevices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, possibleDevices.data());
        
        for (const auto &possibleDevice : possibleDevices) {
            //queryDeviceProperties(possibleDevice);
            if (isDeviceSuitable(possibleDevice)) {
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(possibleDevice, &deviceProperties);
                std::string name = deviceProperties.deviceName;
                if (name.find("llvmpipe") == std::string::npos) // check if device is virtual
                    physicalDevice = possibleDevice;
            }
        }
        
        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
    
    static void queryDeviceProperties(VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        std::cout << "\t* Device: " << device << ":";
        std::cout << "\n\t\t* Name: " << deviceProperties.deviceName;
        std::cout << "\n\t\t* Type: " << deviceProperties.deviceType;
        std::cout << "\n\t\t* ID: " << deviceProperties.deviceID;
        std::cout << "\n\t\t* Vendor ID: " << deviceProperties.vendorID;
        std::cout << "\n\t\t* Driver version: " << deviceProperties.driverVersion;
        std::cout << "\n\t\t* API version: " << deviceProperties.apiVersion;
        std::cout << "\n\t\t* Pipeline cache UUID: " << std::bitset<64>((uint64_t) deviceProperties.pipelineCacheUUID);
        std::cout << "\n\t\t* Sparse properties:";
        std::cout << "\n\t\t\t* Standard 2D block shape:"
                  << deviceProperties.sparseProperties.residencyStandard2DBlockShape;
        std::cout << "\n\t\t\t* Standard 2D multisample block shape:"
                  << deviceProperties.sparseProperties.residencyStandard2DMultisampleBlockShape;
        std::cout << "\n\t\t\t* Standard 3D block shape:"
                  << deviceProperties.sparseProperties.residencyStandard3DBlockShape;
        std::cout << "\n\t\t\t* Aligned MIP size:" << deviceProperties.sparseProperties.residencyAlignedMipSize;
        std::cout << "\n\t\t\t* Non resident strict:" << deviceProperties.sparseProperties.residencyNonResidentStrict;
        std::cout << "\n\t\t* Limits:";
        std::cout << "\n\t\t\t* Max image dimension 1D: " << deviceProperties.limits.maxImageDimension1D;
        std::cout << "\n\t\t\t* Max image dimension 2D: " << deviceProperties.limits.maxImageDimension2D;
        std::cout << "\n\t\t\t* Max image dimension 3D: " << deviceProperties.limits.maxImageDimension3D;
        std::cout << "\n\t\t\t* Max image dimension cube: " << deviceProperties.limits.maxImageDimensionCube;
        std::cout << "\n\t\t\t* Max image array layers: " << deviceProperties.limits.maxImageArrayLayers;
        std::cout << "\n\t\t\t* Max texel buffer elements: " << deviceProperties.limits.maxTexelBufferElements;
        std::cout << "\n\t\t\t* Max uniform buffer range: " << deviceProperties.limits.maxUniformBufferRange;
        std::cout << "\n\t\t\t* Max storage buffer range: " << deviceProperties.limits.maxStorageBufferRange;
        std::cout << "\n\t\t\t* Max push constants size: " << deviceProperties.limits.maxPushConstantsSize;
        std::cout << "\n\t\t\t* Max memory allocation count: " << deviceProperties.limits.maxMemoryAllocationCount;
        std::cout << "\n\t\t\t* Max sampler allocation count: " << deviceProperties.limits.maxSamplerAllocationCount;
        std::cout << "\n\t\t\t* Buffer image granularity: " << deviceProperties.limits.bufferImageGranularity;
        std::cout << "\n\t\t\t* Sparse address space size: " << deviceProperties.limits.sparseAddressSpaceSize;
        std::cout << "\n\t\t\t* Max bound descriptor sets: " << deviceProperties.limits.maxBoundDescriptorSets;
        std::cout << "\n\t\t\t* Max per stage descriptor samplers: "
                  << deviceProperties.limits.maxPerStageDescriptorSamplers;
        std::cout << "\n\t\t\t* Max per stage descriptor uniform buffers: "
                  << deviceProperties.limits.maxPerStageDescriptorUniformBuffers;
        std::cout << "\n\t\t\t* Max per stage descriptor storage buffers: "
                  << deviceProperties.limits.maxPerStageDescriptorStorageBuffers;
        std::cout << "\n\t\t\t* Max per stage descriptor sampled images: "
                  << deviceProperties.limits.maxPerStageDescriptorSampledImages;
        std::cout << "\n\t\t\t* Max per stage descriptor storage images: "
                  << deviceProperties.limits.maxPerStageDescriptorStorageImages;
        std::cout << "\n\t\t\t* Max per stage descriptor input attachments: "
                  << deviceProperties.limits.maxPerStageDescriptorInputAttachments;
        std::cout << "\n\t\t\t* Max per stage resources: " << deviceProperties.limits.maxPerStageResources;
        std::cout << "\n\t\t\t* Max descriptor set samplers: " << deviceProperties.limits.maxDescriptorSetSamplers;
        std::cout << "\n\t\t\t* Max descriptor set uniform buffers: "
                  << deviceProperties.limits.maxDescriptorSetUniformBuffers;
        std::cout << "\n\t\t\t* Max descriptor set uniform buffers [dynamic]: "
                  << deviceProperties.limits.maxDescriptorSetUniformBuffersDynamic;
        std::cout << "\n\t\t\t* Max descriptor set storage buffers: "
                  << deviceProperties.limits.maxDescriptorSetStorageBuffers;
        std::cout << "\n\t\t\t* Max descriptor set storage buffers [dynamic]: "
                  << deviceProperties.limits.maxDescriptorSetStorageBuffersDynamic;
        std::cout << "\n\t\t\t* Max descriptor set sampled images: "
                  << deviceProperties.limits.maxDescriptorSetSampledImages;
        std::cout << "\n\t\t\t* Max descriptor set storage images: "
                  << deviceProperties.limits.maxDescriptorSetStorageImages;
        std::cout << "\n\t\t\t* Max descriptor set input attachments: "
                  << deviceProperties.limits.maxDescriptorSetInputAttachments;
        std::cout << "\n\t\t\t* Max vertex input attributes: " << deviceProperties.limits.maxVertexInputAttributes;
        std::cout << "\n\t\t\t* Max vertex input attribute offset: "
                  << deviceProperties.limits.maxVertexInputAttributeOffset;
        std::cout << "\n\t\t\t* Max vertex input bindings: " << deviceProperties.limits.maxVertexInputBindings;
        std::cout << "\n\t\t\t* Max vertex input binding stride: "
                  << deviceProperties.limits.maxVertexInputBindingStride;
        std::cout << "\n\t\t\t* Max vertex output components: " << deviceProperties.limits.maxVertexOutputComponents;
        std::cout << "\n\t\t\t* Max tessellation generation level: "
                  << deviceProperties.limits.maxTessellationGenerationLevel;
        std::cout << "\n\t\t\t* Max tessellation patch size: " << deviceProperties.limits.maxTessellationPatchSize;
        std::cout << "\n\t\t\t* Max tessellation control per vertex input components: "
                  << deviceProperties.limits.maxTessellationControlPerVertexInputComponents;
        std::cout << "\n\t\t\t* Max tessellation control per vertex output components: "
                  << deviceProperties.limits.maxTessellationControlPerVertexOutputComponents;
        std::cout << "\n\t\t\t* Max tessellation control per patch output components: "
                  << deviceProperties.limits.maxTessellationControlPerPatchOutputComponents;
        std::cout << "\n\t\t\t* Max tessellation control total output components: "
                  << deviceProperties.limits.maxTessellationControlTotalOutputComponents;
        std::cout << "\n\t\t\t* Max tessellation evaluation input components: "
                  << deviceProperties.limits.maxTessellationEvaluationInputComponents;
        std::cout << "\n\t\t\t* Max tessellation evaluation output components: "
                  << deviceProperties.limits.maxTessellationEvaluationOutputComponents;
        std::cout << "\n\t\t\t* Max geometry shader invocations: "
                  << deviceProperties.limits.maxGeometryShaderInvocations;
        std::cout << "\n\t\t\t* Max geometry input components: " << deviceProperties.limits.maxGeometryInputComponents;
        std::cout << "\n\t\t\t* Max geometry output components: "
                  << deviceProperties.limits.maxGeometryOutputComponents;
        std::cout << "\n\t\t\t* Max geometry output vertices: " << deviceProperties.limits.maxGeometryOutputVertices;
        std::cout << "\n\t\t\t* Max geometry total output components: "
                  << deviceProperties.limits.maxGeometryTotalOutputComponents;
        std::cout << "\n\t\t\t* Max fragment input components: " << deviceProperties.limits.maxFragmentInputComponents;
        std::cout << "\n\t\t\t* Max fragment output attachments: "
                  << deviceProperties.limits.maxFragmentOutputAttachments;
        std::cout << "\n\t\t\t* Max fragment dual src attachments: "
                  << deviceProperties.limits.maxFragmentDualSrcAttachments;
        std::cout << "\n\t\t\t* Max fragment combined output resources: "
                  << deviceProperties.limits.maxFragmentCombinedOutputResources;
        std::cout << "\n\t\t\t* Max compute shared memory size: " << deviceProperties.limits.maxComputeSharedMemorySize;
        std::cout << "\n\t\t\t* Max compute work group count: " << deviceProperties.limits.maxComputeWorkGroupCount[0]
                  << " x " << deviceProperties.limits.maxComputeWorkGroupCount[1] << " x "
                  << deviceProperties.limits.maxComputeWorkGroupCount[2];
        std::cout << "\n\t\t\t* Max compute work group invocations: "
                  << deviceProperties.limits.maxComputeWorkGroupInvocations;
        std::cout << "\n\t\t\t* Max compute work group size: " << deviceProperties.limits.maxComputeWorkGroupSize[0]
                  << " x " << deviceProperties.limits.maxComputeWorkGroupSize[1] << " x "
                  << deviceProperties.limits.maxComputeWorkGroupSize[2];
        std::cout << "\n\t\t\t* Sub pixel precision bits: " << deviceProperties.limits.subPixelPrecisionBits;
        std::cout << "\n\t\t\t* Sub texel precision bits: " << deviceProperties.limits.subTexelPrecisionBits;
        std::cout << "\n\t\t\t* Max mipmap precision bits: " << deviceProperties.limits.mipmapPrecisionBits;
        std::cout << "\n\t\t\t* Max draw indexed value count: " << deviceProperties.limits.maxDrawIndexedIndexValue;
        std::cout << "\n\t\t\t* Max draw indirect count: " << deviceProperties.limits.maxDrawIndirectCount;
        std::cout << "\n\t\t\t* Max sampler LOD bias: " << deviceProperties.limits.maxSamplerLodBias;
        std::cout << "\n\t\t\t* Max sampler anisotropy: " << deviceProperties.limits.maxSamplerAnisotropy;
        std::cout << "\n\t\t\t* Max viewports: " << deviceProperties.limits.maxViewports;
        std::cout << "\n\t\t\t* Max viewport dimensions: " << deviceProperties.limits.maxViewportDimensions[0] << " x "
                  << deviceProperties.limits.maxViewportDimensions[1];
        std::cout << "\n\t\t\t* Viewport bounds range: " << deviceProperties.limits.viewportBoundsRange[0] << " - "
                  << deviceProperties.limits.viewportBoundsRange[1];
        std::cout << "\n\t\t\t* Viewport sub pixel bits: " << deviceProperties.limits.viewportSubPixelBits;
        std::cout << "\n\t\t\t* Min memory map alignment: " << deviceProperties.limits.minMemoryMapAlignment;
        std::cout << "\n\t\t\t* Min texel buffer offset alignment: "
                  << deviceProperties.limits.minTexelBufferOffsetAlignment;
        std::cout << "\n\t\t\t* Min uniform buffer offset alignment: "
                  << deviceProperties.limits.minUniformBufferOffsetAlignment;
        std::cout << "\n\t\t\t* Min storage buffer offset alignment: "
                  << deviceProperties.limits.minStorageBufferOffsetAlignment;
        std::cout << "\n\t\t\t* Min texel offset: " << deviceProperties.limits.minTexelOffset;
        std::cout << "\n\t\t\t* Max texel offset: " << deviceProperties.limits.maxTexelOffset;
        std::cout << "\n\t\t\t* Min texel gather offset: " << deviceProperties.limits.minTexelGatherOffset;
        std::cout << "\n\t\t\t* Max texel gather offset: " << deviceProperties.limits.maxTexelGatherOffset;
        std::cout << "\n\t\t\t* Min interpolation offset: " << deviceProperties.limits.minInterpolationOffset;
        std::cout << "\n\t\t\t* Max interpolation offset: " << deviceProperties.limits.minInterpolationOffset;
        std::cout << "\n\t\t\t* Sub pixel interpolation offset bits: "
                  << deviceProperties.limits.subPixelInterpolationOffsetBits;
        std::cout << "\n\t\t\t* Max framebuffer width: " << deviceProperties.limits.maxFramebufferWidth;
        std::cout << "\n\t\t\t* Max framebuffer height: " << deviceProperties.limits.maxFramebufferHeight;
        std::cout << "\n\t\t\t* Max framebuffer layers: " << deviceProperties.limits.maxFramebufferLayers;
        std::cout << "\n\t\t\t* Framebuffer color sample counts: "
                  << deviceProperties.limits.framebufferColorSampleCounts;
        std::cout << "\n\t\t\t* Framebuffer depth sample counts: "
                  << deviceProperties.limits.framebufferDepthSampleCounts;
        std::cout << "\n\t\t\t* Framebuffer stencil sample counts: "
                  << deviceProperties.limits.framebufferStencilSampleCounts;
        std::cout << "\n\t\t\t* Framebuffer no attachment sample counts: "
                  << deviceProperties.limits.framebufferNoAttachmentsSampleCounts;
        std::cout << "\n\t\t\t* Max color attachments: " << deviceProperties.limits.maxColorAttachments;
        std::cout << "\n\t\t\t* Sampled image color sample counts: "
                  << deviceProperties.limits.sampledImageColorSampleCounts;
        std::cout << "\n\t\t\t* Sampled image integer sample counts: "
                  << deviceProperties.limits.sampledImageIntegerSampleCounts;
        std::cout << "\n\t\t\t* Sampled image depth sample counts: "
                  << deviceProperties.limits.sampledImageDepthSampleCounts;
        std::cout << "\n\t\t\t* Sampled image stencil sample counts: "
                  << deviceProperties.limits.sampledImageStencilSampleCounts;
        std::cout << "\n\t\t\t* Storage image sample counts: " << deviceProperties.limits.storageImageSampleCounts;
        std::cout << "\n\t\t\t* Max sample mask words: " << deviceProperties.limits.maxSampleMaskWords;
        std::cout << "\n\t\t\t* Timestamp compute and graphic: " << deviceProperties.limits.timestampComputeAndGraphics;
        std::cout << "\n\t\t\t* Timestamp period: " << deviceProperties.limits.timestampPeriod;
        std::cout << "\n\t\t\t* Max clip distances: " << deviceProperties.limits.maxClipDistances;
        std::cout << "\n\t\t\t* Max cull distances: " << deviceProperties.limits.maxCullDistances;
        std::cout << "\n\t\t\t* Max combined clip and cull distances: "
                  << deviceProperties.limits.maxCombinedClipAndCullDistances;
        std::cout << "\n\t\t\t* Discrete queue priorities: " << deviceProperties.limits.discreteQueuePriorities;
        std::cout << "\n\t\t\t* Point size range: " << deviceProperties.limits.pointSizeRange[0] << " - "
                  << deviceProperties.limits.pointSizeRange[1];
        std::cout << "\n\t\t\t* Point size granularity: " << deviceProperties.limits.pointSizeGranularity;
        std::cout << "\n\t\t\t* Line width range: " << deviceProperties.limits.lineWidthRange[0] << " - "
                  << deviceProperties.limits.lineWidthRange[1];
        std::cout << "\n\t\t\t* Line width granularity: " << deviceProperties.limits.lineWidthGranularity;
        std::cout << "\n\t\t\t* Strict lines: " << deviceProperties.limits.strictLines;
        std::cout << "\n\t\t\t* Standard sample locations: " << deviceProperties.limits.standardSampleLocations;
        std::cout << "\n\t\t\t* Optimal buffer copy offset alignment: "
                  << deviceProperties.limits.optimalBufferCopyOffsetAlignment;
        std::cout << "\n\t\t\t* Optimal buffer copy row pitch alignment: "
                  << deviceProperties.limits.optimalBufferCopyRowPitchAlignment;
        std::cout << "\n\t\t\t* Non coherent atom size: " << deviceProperties.limits.nonCoherentAtomSize;
        std::cout << '\n';
    }
    
    void createLogicalDevice() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
        
        std::set<uint32_t> uniqueQueueFamilies = {queueFamilyIndices.presentFamily.value(),
                                                  queueFamilyIndices.graphicsFamily.value()};
        
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(uniqueQueueFamilies.size());
        
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures deviceFeatures{};
        
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        
        createInfo.pEnabledFeatures = &deviceFeatures;
        
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }
        
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }
        
        vkGetDeviceQueue(device, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(device, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
    }
    
    void createSwapChain() {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
        
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }
        
        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        
        QueueFamilyIndices queueIndices = findQueueFamilies(physicalDevice);
        uint32_t queueFamilyIndices[] = {queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value()};
        
        if (queueIndices.graphicsFamily != queueIndices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        
        if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }
        
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
        
        swapChainImageFormat = surfaceFormat.format;
        swapChainExtent = extent;
    }
    
    void createImageViews() {
        swapChainImageViews.resize(swapChainImages.size());
        
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat,
                                                     VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }
    
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlag) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = image;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = aspectFlag;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        
        VkImageView imageView;
        if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view!");
        }
        
        return imageView;
    }
    
    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        
        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        
        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }
    
    void createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        
        VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
        uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        uboLayoutInfo.bindingCount = 1;
        uboLayoutInfo.pBindings = &uboLayoutBinding;
        
        if (vkCreateDescriptorSetLayout(device, &uboLayoutInfo, nullptr, &uboDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }
    
    void createGraphicsPipeline() {
        auto vertShaderCode = readFile("res/shaders/vert_quad.spv");
        auto fragShaderCode = readFile("res/shaders/frag.spv");
        
        VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
        VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
        
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";
        
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        
        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
        
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) swapChainExtent.width;
        viewport.height = (float) swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChainExtent;
        
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        
        std::array<VkDescriptorSetLayout, 1> descriptorSetLayouts = { uboDescriptorSetLayout };
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        
        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &graphicsPipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = graphicsPipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        
        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }
        
        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }
    
    void createFrameBuffers() {
        swapChainFrameBuffers.resize(swapChainImageViews.size());
        
        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                swapChainImageViews[i]
            };
            
            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;
            
            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }
    
    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);
        
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        
        if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics command pool!");
        }
    }
    
    void createGraphicsDescriptorPools() {
        VkDescriptorPoolSize graphicsPoolSize{};
        graphicsPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        graphicsPoolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());
        
        std::array<VkDescriptorPoolSize, 1> descriptorPools = { graphicsPoolSize };
        
        VkDescriptorPoolCreateInfo graphicsPoolInfo{};
        graphicsPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        graphicsPoolInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
        graphicsPoolInfo.pPoolSizes = descriptorPools.data();
        graphicsPoolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
        
        if (vkCreateDescriptorPool(device, &graphicsPoolInfo, nullptr, &graphicsDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
    
    void createImGUIDescriptorPools() {
        VkDescriptorPoolSize pool_sizes[] =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };
        
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        
        if (vkCreateDescriptorPool(device, &pool_info, nullptr, &imguiDescriptorPool) != VK_SUCCESS)
            throw std::runtime_error("Could not create ImGUI descriptor pool");
    }
    
    void createGraphicsDescriptorSets() {
        std::vector<VkDescriptorSetLayout> graphicsLayouts(swapChainImages.size(), uboDescriptorSetLayout);
        
        VkDescriptorSetAllocateInfo graphicsAllocInfo{};
        graphicsAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        graphicsAllocInfo.descriptorPool = graphicsDescriptorPool;
        graphicsAllocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        graphicsAllocInfo.pSetLayouts = graphicsLayouts.data();
        
        graphicsDescriptorSets.resize(swapChainImages.size());
        if (vkAllocateDescriptorSets(device, &graphicsAllocInfo, graphicsDescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate graphics descriptor sets!");
        }
        
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo graphicsBufferInfo{};
            graphicsBufferInfo.buffer = uniformBuffers[i];
            graphicsBufferInfo.offset = 0;
            graphicsBufferInfo.range = sizeof(MandelbrotUBO);
            
            VkWriteDescriptorSet graphicsDescriptorWrite{};
            graphicsDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            graphicsDescriptorWrite.dstSet = graphicsDescriptorSets[i];
            graphicsDescriptorWrite.dstBinding = 0;
            graphicsDescriptorWrite.dstArrayElement = 0;
            graphicsDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            graphicsDescriptorWrite.descriptorCount = 1;
            graphicsDescriptorWrite.pBufferInfo = &graphicsBufferInfo;
            graphicsDescriptorWrite.pImageInfo = nullptr;
            graphicsDescriptorWrite.pTexelBufferView = nullptr;
            
            vkUpdateDescriptorSets(device, 1, &graphicsDescriptorWrite, 0, nullptr);
        }
    }
    
    void initImGUI() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        /*ImGuiIO& io = ImGui::GetIO();
    
        ImFont* font = io.Fonts->AddFontFromFileTTF((AppRootDir + "res/fonts/Roboto-Medium.ttf").c_str(), 15.0f);
        if (font == nullptr)
            std::cout << "Error: Font for ImGui could not be loaded" << std::endl;
        
        ImGui::StyleColorsDark();*/
        
        ImGui_ImplGlfw_InitForVulkan(window, true);
        
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = device;
        init_info.Queue = graphicsQueue;
        init_info.DescriptorPool = imguiDescriptorPool;
        init_info.MinImageCount = static_cast<uint32_t>(swapChainImages.size());
        init_info.ImageCount = static_cast<uint32_t>(swapChainImages.size());
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        
        ImGui_ImplVulkan_Init(&init_info, renderPass);
        
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = commandPool;
        allocateInfo.commandBufferCount = 1;
        
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        
        vkEndCommandBuffer(commandBuffer);
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
        
        ImGui_ImplVulkan_DestroyFontUploadObjects();
        
        vkDestroyDescriptorPool(device, imguiDescriptorPool, nullptr);
        
        ImGui_ImplVulkan_Shutdown();
    }
    
    VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
            
            if ((tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) ||
                (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)) {
                return format;
            }
        }
        
        throw std::runtime_error("failed to find supported format!");
    }
    
    VkFormat findDepthFormat() {
        return findSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                                  );
    }
    
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlagBits usage,
                     VkMemoryPropertyFlags property, VkImage &image, VkDeviceMemory &memory) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0;
        
        if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }
        
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);
        
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }
        
        vkBindImageMemory(device, image, memory, 0);
    }
    
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        
        
        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);
        
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, properties);
        
        if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }
        
        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }
    
    void createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(MandelbrotUBO);
        
        uniformBuffers.resize(swapChainImages.size());
        uniformBuffersMemory.resize(swapChainImages.size());
        
        for (size_t i = 0; i < swapChainImages.size(); i++) {
            createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i],
                         uniformBuffersMemory[i]);
        }
    }
    
    //todo: create separate command pool for copying buffers with "VK_COMMAND_POOL_CREATE_TRANSIENT_BIT"
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = commandPool;
        allocateInfo.commandBufferCount = 1;
        
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
        vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(commandBuffer, &beginInfo);
        
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        vkEndCommandBuffer(commandBuffer);
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        //todo: allow multi queue submit
        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(graphicsQueue);
        
        vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    }
    
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if (((typeFilter & (1 << i)) != 0u) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
            
            throw std::runtime_error("failed to find suitable memory type!");
        }
        return 0;
    }
    
    void setupImGUIFrame() {
    
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    
        if (ImGui::Begin("Options", &ImGuiEnabled, ImGuiWindowFlags_NoCollapse)) {	// Create a window and append into it.
            ImGui::SetWindowSize({0,0}, ImGuiCond_FirstUseEver); // set window to fit contents when first creating it (ImGui saves position bewteen sessions)
        
            if (ImGui::BeginTabBar("#idTabBar"))
            {
                if (ImGui::BeginTabItem("Info"))
                {
                    if (ImGui::SliderInt("Max iterations", &maxIterations, 1, 8000))
                        autoMaxIterations = false;
                    ImGui::Checkbox("auto max iterations", &autoMaxIterations);
                
                    ImGui::Text("Color: ");
                    ImGui::SameLine();
                    if (ImGui::SmallButton("RGB"))
                        color = 0;
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Black/White"))
                        color = 1;
                    ImGui::SameLine();
                    if (ImGui::SmallButton("Glowing"))
                        color = 2;
                
                    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    ImGui::Text("%.1f fps", currentFPS);
                    ImGui::Text("Zoom: %.1Le", zoomScale);
                    auto [real, imag] = getNumberAtCursor();
                    ImGui::Text("Cursor: %.10Lf + %.10Lf i", real, imag);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Saved views"))
                {
                    // Button to save current view
                    if (ImGui::Button("Save current view"))
                        SavedView::saveNew(zoomScale, {posReal, posImg});
                
                    for (SavedView& savedView : SavedView::allViews) {
                        ImGui::PushID(savedView.getImGuiIDs()[0]);
                        if (ImGui::SmallButton("View")) {
                            jumpToView(savedView);
                        }
                        ImGui::PopID();
                        ImGui::SameLine();
                        ImGui::Text("%s", savedView.getName().c_str());
                        ImGui::SameLine();
                    
                        static int editSavedViewID = -1;
                        ImGui::PushID(savedView.getImGuiIDs()[1]);
                        static char buffer[50];
                        if (ImGui::SmallButton("Edit")) {
                            editSavedViewID = savedView.getImGuiIDs()[0];
                            copyStringToBuffer(savedView.getName(), buffer, 50);
                        }
                        ImGui::PopID();
                    
                        if (editSavedViewID == savedView.getImGuiIDs()[0]) {
                            ImGui::OpenPopup("Edit saved view");
                            ImGui::BeginPopup("Edit saved view"); // TODO https://github.com/ocornut/imgui/issues/2495
                            ImGui::SetWindowSize({150, 0}, ImGuiCond_Once);
                            if (ImGui::InputTextWithHint("Name", "Name", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
                                savedView.setName(buffer);
                                editSavedViewID = -1;
                                copyStringToBuffer("", buffer, 2); // Reset buffer to be empty for next input
                            }
                            // TODO maybe add options to adjust zoom and start number values
                            ImGui::EndPopup();
                        }
                    
                        ImGui::SameLine();
                        ImGui::PushID(savedView.getImGuiIDs()[2]);
                        if (ImGui::SmallButton("Delete"))
                            SavedView::removeSavedView(savedView);
                        ImGui::PopID();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Advanced"))
                {
                    ImGui::Text("Start real:\t%.25Lf", posReal);
                    ImGui::Text("Start imag:\t%.25Lf", posImg);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Help"))
                {
                    ImGui::Text("Press Ctrl + Enter to toggle GUI");
                    ImGui::Text("Press Ctrl + Esc or Pause to exit the app");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
    
        ImGui::End();
    }
    
    void createGraphicsCommandBuffers() {
        graphicsCommandBuffers.resize(swapChainFrameBuffers.size());
        
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) graphicsCommandBuffers.size();
        
        if (vkAllocateCommandBuffers(device, &allocInfo, graphicsCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
        
        for (size_t i = 0; i < graphicsCommandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.pInheritanceInfo = nullptr;
            
            if (vkBeginCommandBuffer(graphicsCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFrameBuffers[i];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChainExtent;
            
            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;
            
            vkCmdBeginRenderPass(graphicsCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            
            vkCmdBindPipeline(graphicsCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
                              graphicsPipeline);
            
            std::array<VkDescriptorSet, 1> descriptorSets = { graphicsDescriptorSets[i] };
            
            vkCmdBindDescriptorSets(graphicsCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout,
                                    0, descriptorSets.size(), descriptorSets.data(), 0, nullptr);
            
            vkCmdDraw(graphicsCommandBuffers[i], 4, 1, 0, 0);
            
            //ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), graphicsCommandBuffers[i]);
            
            vkCmdEndRenderPass(graphicsCommandBuffers[i]);
            
            if (vkEndCommandBuffer(graphicsCommandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }
    
    void createSyncObjects() {
        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                
                throw std::runtime_error("failed to create synchronisation objects for a frame!");
            }
        }
    }
    
    void buildCommandBuffers() {
    
    }
    
    void drawFrame() {
        //ImGui::Render();
        
        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        
        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
                                                imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
                                                &imageIndex);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
    
        updateUniformBuffer(imageIndex);
    
        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];
        
        updateWindowSize();
        updateUniformBuffer(imageIndex);
        currentFPS = getAverageFramerate();
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &graphicsCommandBuffers[imageIndex];
        
        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        
        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        
        vkResetFences(device, 1, &inFlightFences[currentFrame]);
        
        if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        
        VkSwapchainKHR swapChains[] = {swapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        
        presentInfo.pImageIndices = &imageIndex;
        
        result = vkQueuePresentKHR(presentQueue, &presentInfo);
        
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
        
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    
    void updateUniformBuffer(uint32_t currentImage) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        
        auto currentTime = std::chrono::high_resolution_clock::now();
        double time = std::chrono::duration<double, std::chrono::seconds::period>(currentTime - startTime).count();
        
        MandelbrotUBO ubo{};
        ubo.pos = glm::highp_vec2(posReal, posImg);
        ubo.res = glm::mediump_ivec2(windowHeight, windowWidth);
        ubo.zoom = static_cast<double>(zoomScale);
        ubo.iterations = 300;
        
        ubo.time = time;
        
        void *data;
        vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
    }
    
    void updateWindowSize() {
        glfwGetWindowSize(window, &windowHeight, &windowWidth);
    }
    
    float getAverageFramerate() {
        float average = 0.0f;
        for (float value : lastFrameDeltas)
            average += value;
        return average / lastFrameDeltas.size();
    }
    
    VkShaderModule createShaderModule(const std::vector<char> &code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
        
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
        
        return shaderModule;
    }
    
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        
        return availableFormats[0];
    }
    
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }
        
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };
            
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                            capabilities.maxImageExtent.height);
            return actualExtent;
        }
    }
    
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice thisDevice) {
        SwapChainSupportDetails details;
        
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(thisDevice, surface, &details.capabilities);
        
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(thisDevice, surface, &formatCount, nullptr);
        
        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(thisDevice, surface, &formatCount, details.formats.data());
        }
        
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(thisDevice, surface, &presentModeCount, nullptr);
        
        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(thisDevice, surface, &presentModeCount,
                                                      details.presentModes.data());
        }
        
        return details;
    }
    
    bool isDeviceSuitable(VkPhysicalDevice thisDevice) {
        QueueFamilyIndices queueIndices = findQueueFamilies(thisDevice);
        
        bool extensionsSupported = checkDeviceExtensionSupport(thisDevice);
        
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(thisDevice);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }
        
        return queueIndices.isComplete() && extensionsSupported && swapChainAdequate;
    }
    
    static bool checkDeviceExtensionSupport(VkPhysicalDevice thisDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(thisDevice, nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(thisDevice, nullptr, &extensionCount, availableExtensions.data());
        
        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
        
        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }
        
        return requiredExtensions.empty();
    }
    
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice thisDevice) {
        QueueFamilyIndices queueIndices;
        
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(thisDevice, &queueFamilyCount, nullptr);
        
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(thisDevice, &queueFamilyCount, queueFamilies.data());
        
        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueIndices.graphicsFamily = i;
            }
            
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(thisDevice, i, surface, &presentSupport);
            
            if (presentSupport) {
                queueIndices.presentFamily = i;
            }
            
            if (queueIndices.isComplete()) {
                break;
            }
            
            i++;
        }
        
        return queueIndices;
    }
    
    static std::vector<const char *> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        
        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        
        return extensions;
    }
    
    static bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        std::cout << "\navailable layers:\n";
        for (const auto &layers : availableLayers) {
            std::cout << "\t* " << layers.layerName << '\n';
        }
        
        std::cout << "\nrequired layers:\n";
        for (const char *layers : validationLayers) {
            std::cout << "\t* " << layers << '\n';
        }
        
        for (const char *layerName : validationLayers) {
            bool layerFound = false;
            
            for (const auto &layerProperties : availableLayers) {
                if (!strcmp(layerName, layerProperties.layerName)) {
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
    
    static std::vector<char> readFile(const std::string &filename) {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file!");
        }
        
        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        
        file.close();
        
        return buffer;
    }
    
    static VKAPI_ATTR VKAPI_CALL VkBool32 debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData
                                                       ) {
        
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        
        return VK_FALSE;
    }
    
    static bool checkExtensionSupport() {
        uint32_t extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        
        std::vector<const char *> vecGlfwExtensions = getRequiredExtensions();
        
        if (debugLevel > 0) {
            
            std::cout << "\navailable extensions:\n";
            for (const auto &extensionProperties : availableExtensions) {
                std::cout << "\t* " << extensionProperties.extensionName << '\n';
            }
            
            std::cout << "\nrequired extensions:\n";
            for (const char *extensionName : vecGlfwExtensions) {
                std::cout << "\t* " << extensionName << '\n';
            }
        }
        
        for (const char *extensionName : vecGlfwExtensions) {
            bool extensionFound = false;
            
            for (const auto &extensionProperties : availableExtensions) {
                if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
                    extensionFound = true;
                    break;
                }
            }
            
            if (!extensionFound) {
                return false;
            }
        }
        
        return true;
    }
    
    void zoom(long double factor) {
        ComplexNum mouse = getNumberAtCursor();
        
        posReal += (1.0L - factor) * zoomScale * (mouse.first / windowHeight);
        posImg += (1.0L - factor) * zoomScale * (mouse.second / windowWidth);
        
        zoomScale *= factor;
    }
    
    void jumpToView(const SavedView& savedView) {
        zoomScale = savedView.getZoomScale();
        ComplexNum pos = savedView.getStartNum();
        posReal = pos.first;
        posImg = pos.second;
    }
    
    int getMaxIterations() {
        //int zoomCount = -std::log(zoomScale / 3.5) / std::log(ZOOM_STEP); // how often you have zoomed in
        
        if (autoMaxIterations) {
            maxIterations = static_cast<int>(400 + 100L * -std::log10(zoomScale));
            if (maxIterations < 200)
                maxIterations = 200;
            else if (maxIterations > 4000)
                maxIterations = 4000;
        }
        return maxIterations;
    }
    
    ComplexNum getNumberAtCursor() {
        double mouseX = 0.0;
        double mouseY = 0.0;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        return {mouseX, mouseY};
    }
    
    void toggleImGUI() {
        ImGuiEnabled = !ImGuiEnabled;
    }
};

int main() {
    
    MandelbrotApp app;
    
    try {
        app.run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    std::cout << "application closed\n";
    
    return EXIT_SUCCESS;
}
