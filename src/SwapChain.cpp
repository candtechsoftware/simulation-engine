#include "SwapChain.hpp"

#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Simulation {
SwapChain::SwapChain(Device& device_ref, VkExtent2D window_extent)
    : m_device { device_ref }
    , m_window_extent { window_extent }
{
    create_swap_chain();
    create_image_views();
    create_render_pass();
    create_depth_resources();
    create_frame_buffers();
    create_sync_objects();
}
SwapChain::~SwapChain()
{
    for (auto image_view : m_swap_chain_image_views)
        vkDestroyImageView(m_device.device(), image_view, nullptr);

    m_swap_chain_image_views.clear();

    if (m_swap_chain != nullptr) {
        vkDestroySwapchainKHR(m_device.device(), m_swap_chain, nullptr);
        m_swap_chain = nullptr;
    }

    for (size_t i = 0; i < m_depth_images.size(); i++) {
        vkDestroyImageView(m_device.device(), m_depth_image_views[i], nullptr);
        vkDestroyImage(m_device.device(), m_depth_images[i], nullptr);
        vkFreeMemory(m_device.device(), m_depth_image_memorys[i], nullptr);
    }

    for (auto frame_buffer : m_swap_chain_frame_buffers) {
        vkDestroyFramebuffer(m_device.device(), frame_buffer, nullptr);
    }

    vkDestroyRenderPass(m_device.device(), m_render_pass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(m_device.device(), m_render_finished_semaphores[i], nullptr);
        vkDestroySemaphore(m_device.device(), m_image_available_semaphores[i], nullptr);
        vkDestroyFence(m_device.device(), m_in_flight_fences[i], nullptr);
    }
}

} // namespace Simulation
