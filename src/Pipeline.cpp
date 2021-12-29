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
    return config_info;
}
} // namespace Simulation
