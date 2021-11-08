#include "Window.hpp"

#include <GLFW/glfw3.h>

namespace Simulation {
Window::Window(int width, int height, const std::string& name)
    : m_width(width), m_height(height), m_name(name) {
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window =
        glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
}

}  // namespace Simulation
