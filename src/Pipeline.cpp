#include "Pipeline.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace Simulation {
Pipeline::Pipeline(const std::string& vertex_filepath, const std::string& frag_filepath)
{
    create_graphics_pipeline(vertex_filepath, frag_filepath);
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
    const std::string& vertex_filepath, const std::string& frag_filepath)
{
    auto v_code = read_file(vertex_filepath);
    auto f_code = read_file(frag_filepath);

    std::cout << "V " << v_code.size() << std::endl;
    std::cout << "F " << f_code.size() << std::endl;
}
} // namespace Simulation
