#include "Application.hpp"

#include <GLFW/glfw3.h>

namespace Simulation {

void Application::run() {
    while (!m_window.shouldClose()) {
        glfwPollEvents();
    }
}
}  // namespace Simulation
