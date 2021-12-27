#pragma once

#include "Window.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Simulation {

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

struct QueueFamilyIndicies {
    uint32_t graphics_family;
    uint32_t present_family;
    bool graphics_family_has_value = false;
    bool present_family_has_value = false;
    bool is_complete() { return graphics_family_has_value && present_family_has_value; }
};

class Device {
public:
#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

    Device(Window& window);
    ~Device();

    // Not copyable or movable
    Device(const Device&) = delete;
    void operator=(const Device&) = delete;
    Device(const Device&&) = delete;
    Device& operator=(const Device&&) = delete;

    VkCommandPool get_command_pool() { return m_command_pool; }
    VkDevice device() { return m_device; }
    VkSurfaceKHR surface() { return m_surface; };
    VkQueue graphics_queue() { return m_graphics_queue; }
    VkQueue present_queue() { return m_present_queue; }

    SwapChainSupportDetails get_swap_chain_support()
    {
        return query_swap_chain_support(m_physical_device);
    }
    uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
    QueueFamilyIndicies find_physical_queue_families()
    {
        return find_queue_families(m_physical_device);
    }
    VkFormat find_support_format(
        const std::vector<VkFormat>& candidate, VkImageTiling tiling, VkFormatFeatureFlags featues);

    void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);
    VkCommandBuffer being_single_time_commands();
    void end_single_time_commands(VkCommandBuffer command_buffer);
    void copy_buffer(VkBuffer scr, VkBuffer dest, VkDevice size);
    void copy_buffer_to_image(
        VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);
    void create_image_with_info(const VkImageCreateInfo& image_info,
        VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory);
    VkPhysicalDeviceProperties properties;

private:
    void create_instance();
    void setup_debug_messenger();
    void create_surface();
    void pick_physcial_device();
    void create_logical_device();
    void create_command_pool();
    // helper methods
    bool is_device_suitable(VkPhysicalDevice device);
    std::vector<const char*> get_required_ext();
    bool check_validation_layer_support();
    QueueFamilyIndicies find_queue_families(VkPhysicalDevice device);
    void populate_debug_messanger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
    void has_glfw_required_ext();
    bool check_device_ext_support(VkPhysicalDevice device);
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);

    // private members
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debug_messenger;
    VkPhysicalDevice m_physical_device;
    VkCommandPool m_command_pool;
    Window& m_window;
    VkDevice m_device;
    VkSurfaceKHR m_surface;
    VkQueue m_graphics_queue;
    VkQueue m_present_queue;

    const std::vector<const char*> m_validation_layers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> m_device_ext = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
};

} // namespace Simulation
