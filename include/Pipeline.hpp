#pragma once

#include "Device.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

namespace Simulation {

struct PipelineConfigInfo {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewport_info;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineRasterizationStateCreateInfo rasterization_info;
    VkPipelineMultisampleStateCreateInfo multisample_info;
    VkPipelineColorBlendAttachmentState color_blend_attatchment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_info;
    VkPipelineLayout pipeline_layout = nullptr;
    VkRenderPass render_pass = nullptr;
    uint32_t sub_pass = 0;
};

class Pipeline {
public:
    Pipeline(Device& device, const std::string& vertex_filepath, const std::string& frag_filepath,
        const PipelineConfigInfo& config_info);

    ~Pipeline() {};

    Pipeline(const Pipeline&) = delete;
    void operator=(const Pipeline&) = delete;

    static PipelineConfigInfo default_pipeline_config_info(uint32_t width, uint32_t height);

private:
    static std::vector<char> read_file(const std::string& filepath);

    void create_graphics_pipeline(
        const std::string& vertex_filepath, const std::string& frag_filepath, const PipelineConfigInfo& config_info);

    void create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module);

    Device& m_device;
    VkPipeline m_graphics_pipeline;
    VkShaderModule m_vert_shader_module;
    VkShaderModule m_frag_shader_module;
};
} // namespace Simulation
