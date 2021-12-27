#include "Device.hpp"

#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vulkan/vulkan_core.h>

namespace Simulation {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult create_debug_utils_messenger_ext(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator,
    VkDebugUtilsMessengerEXT* debug_messenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDebugUtilsMessengerCreateInfoEXT");
    if (func != nullptr) {
        return func(instance, create_info, allocator, debug_messenger);
    }
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroy_debug_utils_messenger_ext(VkInstance instance,
    VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* allocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, debug_messenger, allocator);
    }
    std::cerr << "failed to destory debug utils messenger EXT\n";
}

Device::Device(Window& window)
    : m_window { window }
{
    create_instance();
    setup_debug_messenger();
    create_surface();
    pick_physcial_device();
    create_logical_device();
    create_command_pool();
}

Device::~Device()
{
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    vkDestroyDevice(m_device, nullptr);

    if (enable_validation_layers) {
        destroy_debug_utils_messenger_ext(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    vkDestroyInstance(m_instance, nullptr);
}

void Device::create_instance()
{
    if (enable_validation_layers && !check_validation_layer_support()) {
        throw std::runtime_error("validation layers requests, but not available!");
    }

    VkApplicationInfo app_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Simulation Engine",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_0,
    };

    auto extensions = get_required_ext();
    VkInstanceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &app_info,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };
    VkDebugUtilsMessengerEXT debug_create_info;
    if (enable_validation_layers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
        create_info.ppEnabledLayerNames = m_validation_layers.data();
        populate_debug_messanger_create_info(debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debug_create_info;
    } else {
        create_info.enabledLayerCount = 0;
        create_info.ppEnabledLayerNames = nullptr;
    }

    if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instnace");
    }
    has_glfw_required_ext();
}

void Device::pick_physcial_device()
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    if (device_count == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support");
    }
    std::cout << "Device count: " << device_count << std::endl;
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());

    for (const auto& device : devices) {
        if (is_device_suitable(device)) {
            m_physical_device = device;
            break;
        }
    }

    if (m_physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU with Vulkan support");
    }

    vkGetPhysicalDeviceProperties(m_physical_device, &properties);
    std::cout << "physical device: " << properties.deviceName << std::endl;
}

void Device::create_logical_device()
{
    QueueFamilyIndicies indicies = find_queue_families(m_physical_device);

    std::vector<VkDeviceQueueCreateInfo> create_info_queue;
    std::set<uint32_t> unique_que_familes = { indicies.graphics_family, indicies.present_family };

    float queue_priority = 1.0f;
    for (uint32_t family : unique_que_familes) {
        VkDeviceQueueCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = family,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority,
        };
        create_info_queue.push_back(create_info);
    }

    VkPhysicalDeviceFeatures device_featues = { .samplerAnisotropy = VK_TRUE };

    VkDeviceCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = static_cast<uint32_t>(create_info_queue.size()),
        .pQueueCreateInfos = create_info_queue.data(),
        .pEnabledFeatures = &device_featues,
    };
    create_info.ppEnabledExtensionNames = m_device_ext.data();
    create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_ext.size());

    if (enable_validation_layers) {
        create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
        create_info.ppEnabledLayerNames = m_validation_layers.data();
    } else {
        create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(m_device, indicies.graphics_family, 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, indicies.present_family, 0, &m_present_queue);
}

void Device::create_command_pool()
{
    QueueFamilyIndicies indicies = find_physical_queue_families();

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = indicies.graphics_family,
    };
    pool_info.flags
        = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(m_device, &pool_info, nullptr, &m_command_pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool");
    }
}

void Device::create_surface() { m_window.create_window_surface(m_instance, &m_surface); }

bool Device::is_device_suitable(VkPhysicalDevice device)
{
    QueueFamilyIndicies indicies = find_queue_families(device);

    bool ext_supported = check_device_ext_support(device);

    bool swap_chain_adequate = false;

    if (ext_supported) {
        SwapChainSupportDetails details = query_swap_chain_support(device);
        swap_chain_adequate = !details.formats.empty() && !details.present_modes.empty();
    }

    VkPhysicalDeviceFeatures supported_featues;
    vkGetPhysicalDeviceFeatures(device, &supported_featues);

    return indicies.is_complete() && ext_supported && swap_chain_adequate
        && supported_featues.samplerAnisotropy;
}

void Device::populate_debug_messanger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
{
    create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debugCallback;
    create_info.pUserData = nullptr;
}

void Device::setup_debug_messenger()
{
    if (!enable_validation_layers)
        return;
    VkDebugUtilsMessengerCreateInfoEXT create_info;
    populate_debug_messanger_create_info(create_info);
    if (create_debug_utils_messenger_ext(m_instance, &create_info, nullptr, &m_debug_messenger)
        != VK_SUCCESS) {
        throw std::runtime_error("failed to setup debug messenger");
    }
}


} // namespace Simulation
