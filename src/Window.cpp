#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>

namespace Simulation {
Window::Window(int width, int height, const std::string& name)
    : m_width(width)
    , m_height(height)
    , m_name(name)
{
    initWindow();
}

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(m_width, m_height, m_name.c_str(), nullptr, nullptr);
}

void Window::create_window_surface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, surface)) {
        throw std::runtime_error("failed to create window surface glfw");
    }
}

} // namespace Simulation
