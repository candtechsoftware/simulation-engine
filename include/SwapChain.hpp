#pragma once

#include "Device.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Simulation {

class SwapChain {
public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    SwapChain(Device& device_ref, VkExtent2D window_extent);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    void operator=(const SwapChain&) = delete;

    VkFramebuffer get_frame_buffer(int index) { return m_swap_chain_frame_buffers[index]; }
    VkRenderPass get_render_pass() { return m_render_pass; }
    VkImageView get_image_view(int index) { return m_swap_chain_image_views[index]; }
    size_t image_count() { return m_swap_chain_images.size(); }
    VkFormat get_swap_chain_image_format() { return m_swap_chain_image_format; }
    VkExtent2D get_swap_chain_extent() { return m_swap_chain_extent; }
    uint32_t width() { return m_swap_chain_extent.width; }
    uint32_t height() { return m_swap_chain_extent.height; }

    float extent_aspect_ratio()
    {
        return static_cast<float>(m_swap_chain_extent.width) / static_cast<float>(m_swap_chain_extent.height);
    }

    VkFormat find_depth_format();

    VkResult accuire_next_image(uint32_t* image_index);
    VkResult submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* image_index);

    void create_swap_chain();
    void create_image_views();
    void create_depth_resources();
    void create_render_pass();
    void create_frame_buffers();
    void create_sync_objects();

    // Helper methods
    VkSurfaceKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilities2EXT& capabilites);

    VkFormat m_swap_chain_image_format;
    VkExtent2D m_swap_chain_extent;

    std::vector<VkFramebuffer> m_swap_chain_frame_buffers;
    VkRenderPass m_render_pass;

    std::vector<VkImage> m_depth_images;
    std::vector<VkDeviceMemory> m_depth_image_memorys;
    std::vector<VkImageView> m_depth_image_views;
    std::vector<VkImage> m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;

    Device& m_device;
    VkExtent2D m_window_extent;

    VkSwapchainKHR m_swap_chain;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight_fences;

    size_t current_frame = 0;
};

} // namespace Simulation
