#pragma once

#include "Pipeline.hpp"
#include "Window.hpp"

namespace Simulation {
class Application {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 800;

    void run();

private:
    Window m_window { WIDTH, HEIGHT, "Hello Vulkan" };
    Device m_device { m_window };
    Pipeline m_pipeline { m_device, "../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv",
        Pipeline::default_pipeline_config_info(WIDTH, HEIGHT) };
};
} // namespace Simulation
