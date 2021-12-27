#pragma once

#include <string>
#include <vector>

namespace Simulation {
class Pipeline {
public:
    Pipeline(const std::string& vertex_filepath, const std::string& frag_filepath);

private:
    static std::vector<char> read_file(const std::string& filepath);

    void create_graphics_pipeline(
        const std::string& vertex_filepath, const std::string& frag_filepath);
};
} // namespace Simulation
