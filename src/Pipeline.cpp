#include "Pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

namespace Simulation {
Pipeline::Pipeline(Device& device, const std::string& vertex_filepath, const std::string& frag_filepath,
    const PipelineConfigInfo& config_info)
    : m_device(device)
{
    create_graphics_pipeline(vertex_filepath, frag_filepath, config_info);
}

std::vector<char> Pipeline::read_file(const std::string& filepath)
{
    std::ifstream file { filepath, std::ios::ate | std::ios::binary };

    if (!file.is_open())
        throw std::runtime_error("failed to open file " + filepath);

    size_t file_size = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
    return buffer;
}

void Pipeline::create_graphics_pipeline(
    const std::string& vertex_filepath, const std::string& frag_filepath, const PipelineConfigInfo& config_info)
{
    auto v_code = read_file(vertex_filepath);
    auto f_code = read_file(frag_filepath);

    std::cout << "V " << v_code.size() << std::endl;
    std::cout << "F " << f_code.size() << std::endl;
}

void Pipeline::create_shader_module(const std::vector<char>& code, VkShaderModule* shader_module)
{
    VkShaderModuleCreateInfo create_info {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(m_device.device(), &create_info, nullptr, shader_module) != VK_SUCCESS) {
        throw std::runtime_error("error creating shader module");
    }
}

PipelineConfigInfo Pipeline::default_pipeline_config_info(uint32_t width, uint32_t height)
{
    PipelineConfigInfo config_info {};

    config_info.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config_info.input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config_info.input_assembly_info.primitiveRestartEnable = VK_FALSE;

    config_info.viewport.x = 0.0f;
    config_info.viewport.y = 0.0f;
    config_info.viewport.width = static_cast<float>(width);
    config_info.viewport.height = static_cast<float>(height);
    config_info.viewport.minDepth = 0.0f;
    config_info.viewport.maxDepth = 1.0f;

    config_info.scissor.offset = { 0, 0 };
    config_info.scissor.extent = { width, height };

    config_info.viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config_info.viewport_info.viewportCount = 1;
    config_info.viewport_info.pViewports = &config_info.viewport;
    config_info.viewport_info.scissorCount = 1;
    config_info.viewport_info.pScissors = &config_info.scissor;

    config_info.rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config_info.rasterization_info.depthBiasClamp = VK_FALSE;
    config_info.rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    config_info.rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    config_info.rasterization_info.lineWidth = 1.0f;
    config_info.rasterization_info.cullMode = VK_CULL_MODE_NONE;
    config_info.rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config_info.rasterization_info.depthBiasEnable = VK_FALSE;
    config_info.rasterization_info.depthBiasConstantFactor = 0.0f;
    config_info.rasterization_info.depthBiasClamp = 0.0f;
    config_info.rasterization_info.depthBiasSlopeFactor = 0.0f;

    config_info.multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config_info.multisample_info.sampleShadingEnable = VK_FALSE;
    config_info.multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config_info.multisample_info.minSampleShading = 1.0f;
    config_info.multisample_info.pSampleMask = nullptr;
    config_info.multisample_info.alphaToCoverageEnable = VK_FALSE;
    config_info.multisample_info.alphaToOneEnable = VK_FALSE;

    config_info.color_blend_attatchment.blendEnable
        = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config_info.color_blend_attatchment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    config_info.color_blend_attatchment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    config_info.color_blend_attatchment.colorBlendOp = VK_BLEND_OP_ADD;
    config_info.color_blend_attatchment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    config_info.color_blend_attatchment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    config_info.color_blend_attatchment.alphaBlendOp = VK_BLEND_OP_ADD;

    config_info.color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config_info.color_blend_info.logicOpEnable = VK_FALSE;
    config_info.color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    config_info.color_blend_info.attachmentCount = 1;
    config_info.color_blend_info.pAttachments = &config_info.color_blend_attatchment;
    config_info.color_blend_info.blendConstants[0] = 0.0f;
    config_info.color_blend_info.blendConstants[1] = 0.0f;
    config_info.color_blend_info.blendConstants[2] = 0.0f;
    config_info.color_blend_info.blendConstants[3] = 0.0f;

    config_info.depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config_info.depth_stencil_info.depthTestEnable = VK_TRUE;
    config_info.depth_stencil_info.depthWriteEnable = VK_TRUE;
    config_info.depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
    config_info.depth_stencil_info.minDepthBounds = 0.0f;
    config_info.depth_stencil_info.maxDepthBounds = 1.0f;
    config_info.depth_stencil_info.stencilTestEnable = VK_FALSE;
    config_info.depth_stencil_info.front = {};
    config_info.depth_stencil_info.back = {};

    return config_info;
}
} // namespace Simulation
